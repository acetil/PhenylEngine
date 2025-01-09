#include "mesh_layer.h"

#include <graphics/renderer.h>

#include "../../../../../include/phenyl/asset.h"
#include "core/assets/assets.h"

using namespace phenyl::graphics;

MeshRenderLayer::MeshRenderLayer (core::World& world) : AbstractRenderLayer{0}, meshQuery{world.query<core::GlobalTransform3D, MeshRenderer3D>()} {}

std::string_view MeshRenderLayer::getName () const {
    return "MeshRenderLayer";
}

void MeshRenderLayer::init (Renderer& renderer) {
    this->renderer = &renderer;

    instanceBuffer = renderer.makeBuffer<glm::mat4>(512);
    globalUniform = renderer.makeUniformBuffer<MeshGlobalUniform>();
    bpLight = renderer.makeUniformBuffer<BPLightUniform>();
    //meshMaterial = core::Assets::Load<Material>("resources/phenyl/materials/blinn_phong");
}

void MeshRenderLayer::addSystems (core::PhenylRuntime& runtime) {
    runtime.addSystem<core::Render>("MeshRenderLayer::upload", this, &MeshRenderLayer::uploadSystem);
}

void MeshRenderLayer::uploadSystem (core::PhenylRuntime& runtime) {
    uploadData(runtime.resource<Camera3D>());
}


void MeshRenderLayer::uploadData (Camera3D& camera) {
    meshQuery.each([&] (const core::GlobalTransform3D& transform, MeshRenderer3D& renderer) {
        auto* mesh = renderer.mesh.get();
        auto* matInstance = renderer.material.get();
        PHENYL_DASSERT(mesh);
        //requests.emplace_back(mesh->layout().layoutId, mesh, transform.transform.transformMatrx());
        requests.emplace_back(MeshRenderRequest{
            .layout = mesh->layout().layoutId,
            .mesh = mesh,
            .materialInstance = matInstance,
            .transform = transform.transform.transformMatrx()
        });
    });

    std::ranges::sort(requests, [] (const MeshRenderRequest& lhs, const MeshRenderRequest& rhs) {
        if (lhs.materialInstance != rhs.materialInstance) {
            return lhs.materialInstance < rhs.materialInstance;
        }

        return lhs.layout != rhs.layout ? lhs.layout < rhs.layout : lhs.mesh < rhs.mesh;
    });


    for (std::size_t i = 0; i < requests.size(); i++) {
        auto& req = requests[i];
        instanceBuffer.emplace(req.transform);

        if (instances.empty() || instances.back().mesh != req.mesh || instances.back().materialInstance != req.materialInstance) {
            instances.emplace_back(MeshInstances{
                .mesh = req.mesh,
                .materialInstance = req.materialInstance,
                .instanceOffset = i,
                .numInstances = 1
            });
        } else {
            instances.back().numInstances++;
        }
    }
    instanceBuffer.upload();
    requests.clear();

    globalUniform->view = camera.view();
    globalUniform->projection = camera.projection();
    globalUniform->viewPos = camera.transform.position();
    globalUniform.upload();
}

void MeshRenderLayer::render () {
    PHENYL_DASSERT(renderer);

    bpLight->lightPos = {-2, 0, 0};
    bpLight->lightColor = {1, 1, 1};
    bpLight->ambientColor = {1.0, 1.0, 1.0};
    bpLight.upload();

    for (const auto& instance : instances) {
        //auto& meshPipeline = getPipeline(instance.mesh->layout()); // TODO: ahead of time pipeline creation
        auto* matInstance = instance.materialInstance;
        auto& matPipeline = matInstance->material()->getPipeline(instance.mesh->layout());
        auto& pipeline = matPipeline.pipeline;

        pipeline.bindUniform(matPipeline.globalUniform, globalUniform);
        pipeline.bindUniform(matPipeline.lightUniform, bpLight);

        auto& streams = instance.mesh->streams();
        PHENYL_DASSERT(streams.size() == matPipeline.streamBindings.size());
        for (std::size_t i = 0; i < streams.size(); i++) {
            pipeline.bindBuffer(matPipeline.streamBindings[i], streams[i]);
        }

        pipeline.bindBuffer(matPipeline.modelBinding, instanceBuffer, instance.instanceOffset);
        pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

        matInstance->bind(matPipeline);

        pipeline.renderInstanced(instance.numInstances, instance.mesh->numVertices());
    }

    instances.clear();
    instanceBuffer.clear();
}

// MeshRenderLayer::MeshPipeline& MeshRenderLayer::getPipeline (const MeshLayout& layout) {
//     if (auto it = pipelines.find(layout.layoutId); it != pipelines.end()) {
//         return it->second;
//     }
//
//     UniformBinding globalUniform;
//
//     PHENYL_DASSERT(renderer);
//     auto shader = core::Assets::Load<Shader>("phenyl/shaders/mesh"); // TODO
//     auto builder = renderer->buildPipeline();
//
//     builder.withShader(shader)
//         .withUniform<GlobalUniform>(*shader->uniformLocation("GlobalUniform"), globalUniform);
//
//     std::vector<BufferBinding> streamBindings;
//     for (auto i : layout.streamStrides) {
//         streamBindings.emplace_back();
//
//         builder.withRawBuffer(streamBindings.back(), i);
//     }
//
//     // TODO: material specific
//     BufferBinding instanceBinding;
//     builder.withBuffer<glm::mat4>(instanceBinding, BufferInputRate::INSTANCE);
//
//     unsigned int location = 0;
//     for (auto& i : layout.attributes) {
//         PHENYL_DASSERT(i.stream < streamBindings.size());
//         builder.withAttrib(location++, streamBindings[i.stream], i.type, i.offset);
//     }
//
//     builder.withAttrib<glm::mat4>(location, instanceBinding);
//
//     auto [it, _] = pipelines.emplace(layout.layoutId, MeshPipeline{
//         .pipeline = builder.build(),
//         .globalUniform = globalUniform,
//         .instanceBinding = instanceBinding,
//         .streamBindings = std::move(streamBindings)
//     });
//     return it->second;
// }