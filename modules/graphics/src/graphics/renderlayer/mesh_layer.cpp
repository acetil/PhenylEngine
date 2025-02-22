#include "mesh_layer.h"

#include <graphics/renderer.h>

#include "core/assets/assets.h"

using namespace phenyl::graphics;

MeshRenderLayer::MeshRenderLayer (core::World& world) : AbstractRenderLayer{0}, meshQuery{world.query<core::GlobalTransform3D, MeshRenderer3D>()}, pointLightQuery{world.query<core::GlobalTransform3D, PointLight3D>()}, dirLightQuery{world.query<core::GlobalTransform3D, DirectionalLight3D>()}, spotLightQuery{world.query<core::GlobalTransform3D, SpotLight3D>()} {}

std::string_view MeshRenderLayer::getName () const {
    return "MeshRenderLayer";
}

void MeshRenderLayer::init (Renderer& renderer) {
    this->renderer = &renderer;
    auto& viewport = renderer.getViewport();

    testFb = renderer.makeFrameBuffer(FrameBufferProperties{}, viewport.getResolution().x, viewport.getResolution().y);
    instanceBuffer = renderer.makeBuffer<glm::mat4>(512);
    globalUniform = renderer.makeUniformBuffer<MeshGlobalUniform>();
    bpLight = renderer.makeUniformBuffer<BPLightUniform>();

    //meshMaterial = core::Assets::Load<Material>("resources/phenyl/materials/blinn_phong");

    auto ppShader = core::Assets::Load<Shader>("phenyl/shaders/postprocess/noop");
    BufferBinding vertexBinding;

    postProcessPipeline = renderer.buildPipeline()
        .withShader(ppShader)
        .withBuffer<glm::vec2>(vertexBinding)
        .withAttrib<glm::vec2>(0, vertexBinding)
        .withSampler2D(*ppShader->samplerLocation("frameBuffer"), ppSampler)
        .build();

    ppQuad = renderer.makeBuffer<glm::vec2>(6);
    ppQuad.emplace(-1, -1);
    ppQuad.emplace(1, -1);
    ppQuad.emplace(1, 1);

    ppQuad.emplace(1, 1);
    ppQuad.emplace(-1, 1);
    ppQuad.emplace(-1, -1);

    ppQuad.upload();

    postProcessPipeline.bindBuffer(vertexBinding, ppQuad);
}

void MeshRenderLayer::addSystems (core::PhenylRuntime& runtime) {
    runtime.addSystem<core::Render>("MeshRenderLayer::upload", this, &MeshRenderLayer::uploadSystem);
}

void MeshRenderLayer::uploadSystem (core::PhenylRuntime& runtime) {
    uploadData(runtime.resource<Camera3D>());
}


void MeshRenderLayer::uploadData (Camera3D& camera) {
    testFb.clear();

    gatherGeometry();
    gatherLights();

    globalUniform->view = camera.view();
    globalUniform->projection = camera.projection();
    globalUniform->viewPos = camera.transform.position();
    globalUniform.upload();
}

void MeshRenderLayer::render () {
    PHENYL_DASSERT(renderer);

    depthPrepass();

    for (const auto& light : pointLights) {
        renderLight(light);
    }

    postProcessing();

    instances.clear();
    pointLights.clear();

    instanceBuffer.clear();
}

void MeshRenderLayer::gatherGeometry () {
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
        if (lhs.materialInstance->material() != rhs.materialInstance->material()) {
            return lhs.materialInstance->material() < rhs.materialInstance->material();
        }

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
}

void MeshRenderLayer::gatherLights () {
    pointLightQuery.each([&] (const core::GlobalTransform3D& transform, const PointLight3D& light) {
        pointLights.emplace_back(MeshLight{
            .pos = transform.transform.position(),
            .color = light.color,
            .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
            .brightness = light.brightness,
            .type = LightType::Point
        });
    });

    dirLightQuery.each([&] (const core::GlobalTransform3D& transform, const DirectionalLight3D& light) {
        pointLights.emplace_back(MeshLight{
            .dir = transform.transform.rotation() * core::Quaternion::ForwardVector,
            .color = light.color,
            .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
            .brightness = light.brightness,
            .type = LightType::Directional
        });
    });

    spotLightQuery.each([&] (const core::GlobalTransform3D& transform, const SpotLight3D& light) {
        pointLights.emplace_back(MeshLight{
            .pos = transform.transform.position(),
            .dir = transform.transform.rotation() * core::Quaternion::ForwardVector,
            .color = light.color,
            .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
            .brightness = light.brightness,
            .cosOuter = glm::cos(light.outerAngle),
            .cosInner = glm::cos(light.innerAngle),
            .type = LightType::Spot
        });
    });
}

void MeshRenderLayer::depthPrepass () {
    for (const auto& instance : instances) {
        auto& depthPipeline = instance.materialInstance->material()->getDepthPipeline(instance.mesh->layout());

        auto& pipeline = depthPipeline.pipeline;
        pipeline.bindUniform(depthPipeline.globalUniform, globalUniform);

        auto& streams = instance.mesh->streams();
        PHENYL_DASSERT(streams.size() == depthPipeline.streamBindings.size());
        for (std::size_t i = 0; i < streams.size(); i++) {
            pipeline.bindBuffer(depthPipeline.streamBindings[i], streams[i]);
        }

        pipeline.bindBuffer(depthPipeline.modelBinding, instanceBuffer, instance.instanceOffset);
        pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

        pipeline.renderInstanced(testFb, instance.numInstances, instance.mesh->numVertices());
    }
}

void MeshRenderLayer::renderLight (const MeshLight& light) {
    bpLight->lightPos = light.pos;
    bpLight->lightDir = light.dir;
    bpLight->lightColor = light.color;
    bpLight->ambientColor = light.ambientColor / static_cast<float>(pointLights.size());
    bpLight->brightness = light.brightness;
    bpLight->cosInner = light.cosInner;
    bpLight->cosOuter = light.cosOuter;
    bpLight->lightType = static_cast<int>(light.type);
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

        pipeline.renderInstanced(testFb, instance.numInstances, instance.mesh->numVertices());
    }
}

void MeshRenderLayer::postProcessing () {
    postProcessPipeline.bindSampler(ppSampler, testFb.sampler());
    postProcessPipeline.render(6);
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