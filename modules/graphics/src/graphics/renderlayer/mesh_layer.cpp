#include "mesh_layer.h"

#include <graphics/renderer.h>

#include "core/assets/assets.h"

using namespace phenyl::graphics;

MeshRenderLayer::MeshRenderLayer (core::World& world) : AbstractRenderLayer{0}, meshQuery{world.query<core::GlobalTransform3D, MeshRenderer3D>()} {}

std::string_view MeshRenderLayer::getName () const {
    return "MeshRenderLayer";
}

void MeshRenderLayer::init (Renderer& renderer) {
    this->renderer = &renderer;

    instanceBuffer = renderer.makeBuffer<glm::mat4>(512);
    globalUniform = renderer.makeUniformBuffer<GlobalUniform>();
}

void MeshRenderLayer::addSystems (core::PhenylRuntime& runtime) {
    runtime.addSystem<core::Render>("MeshRenderLayer::upload", this, &MeshRenderLayer::uploadSystem);
}

void MeshRenderLayer::uploadSystem (core::PhenylRuntime& runtime) {
    uploadData(runtime.resource<Camera3D>());
}


void MeshRenderLayer::uploadData (Camera3D& camera) {
    meshQuery.each([&] (const core::GlobalTransform3D& transform, const MeshRenderer3D& renderer) {
        auto* mesh = renderer.mesh.get();
        PHENYL_DASSERT(mesh);
        requests.emplace_back(mesh->layout().layoutId, mesh, transform.transform.transformMatrx());
    });

    std::ranges::sort(requests, [] (const MeshRenderRequest& lhs, const MeshRenderRequest& rhs) {
        return lhs.layout != rhs.layout ? lhs.layout < rhs.layout : lhs.mesh < rhs.mesh;
    });


    for (std::size_t i = 0; i < requests.size(); i++) {
        auto& req = requests[i];
        instanceBuffer.emplace(req.transform);

        if (instances.empty() || instances.back().mesh != req.mesh) {
            instances.emplace_back(req.mesh, i, 1);
        } else {
            instances.back().numInstances++;
        }
    }
    instanceBuffer.upload();
    requests.clear();

    globalUniform->view = camera.view();
    globalUniform->projection = camera.projection();
    globalUniform.upload();
}

void MeshRenderLayer::render () {
    for (const auto& instance : instances) {
        auto& meshPipeline = getPipeline(instance.mesh->layout()); // TODO: ahead of time pipeline creation
        auto& pipeline = meshPipeline.pipeline;

        pipeline.bindUniform(meshPipeline.globalUniform, globalUniform);

        auto& streams = instance.mesh->streams();
        PHENYL_DASSERT(streams.size() == meshPipeline.streamBindings.size());
        for (std::size_t i = 0; i < streams.size(); i++) {
            pipeline.bindBuffer(meshPipeline.streamBindings[i], streams[i]);
        }

        pipeline.bindBuffer(meshPipeline.instanceBinding, instanceBuffer, instance.instanceOffset);
        pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

        pipeline.renderInstanced(instance.numInstances, instance.mesh->numVertices());
    }

    instances.clear();
    instanceBuffer.clear();
}

MeshRenderLayer::MeshPipeline& MeshRenderLayer::getPipeline (const MeshLayout& layout) {
    if (auto it = pipelines.find(layout.layoutId); it != pipelines.end()) {
        return it->second;
    }

    UniformBinding globalUniform;

    PHENYL_DASSERT(renderer);
    auto shader = core::Assets::Load<Shader>("phenyl/shaders/mesh"); // TODO
    auto builder = renderer->buildPipeline();

    builder.withShader(shader)
        .withUniform<GlobalUniform>(*shader->uniformLocation("GlobalUniform"), globalUniform);

    std::vector<BufferBinding> streamBindings;
    for (auto i : layout.streamStrides) {
        streamBindings.emplace_back();

        builder.withRawBuffer(streamBindings.back(), i);
    }

    // TODO: material specific
    BufferBinding instanceBinding;
    builder.withBuffer<glm::mat4>(instanceBinding, BufferInputRate::INSTANCE);

    unsigned int location = 0;
    for (auto& i : layout.attributes) {
        PHENYL_DASSERT(i.stream < streamBindings.size());
        builder.withAttrib(location++, streamBindings[i.stream], i.type, i.offset);
    }

    builder.withAttrib<glm::mat4>(location, instanceBinding);

    auto [it, _] = pipelines.emplace(layout.layoutId, MeshPipeline{
        .pipeline = builder.build(),
        .globalUniform = globalUniform,
        .instanceBinding = instanceBinding,
        .streamBindings = std::move(streamBindings)
    });
    return it->second;
}