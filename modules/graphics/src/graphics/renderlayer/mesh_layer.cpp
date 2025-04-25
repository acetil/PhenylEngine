#include "mesh_layer.h"

#include <graphics/backend/renderer.h>

#include "core/assets/assets.h"

using namespace phenyl::graphics;

MeshRenderLayer::MeshRenderLayer (core::World& world) : AbstractRenderLayer{0}, meshQuery{world.query<core::GlobalTransform3D, MeshRenderer3D>()}, pointLightQuery{world.query<core::GlobalTransform3D, PointLight3D>()}, dirLightQuery{world.query<core::GlobalTransform3D, DirectionalLight3D>()}, spotLightQuery{world.query<core::GlobalTransform3D, SpotLight3D>()} {}

std::string_view MeshRenderLayer::getName () const {
    return "MeshRenderLayer";
}

void MeshRenderLayer::init (Renderer& renderer) {
    this->renderer = &renderer;
    auto& viewport = renderer.getViewport();

    testFb = renderer.makeFrameBuffer(FrameBufferProperties{
        .format = ImageFormat::RGBA,
        .depthFormat = ImageFormat::DEPTH24_STENCIL8
    }, viewport.getResolution().x, viewport.getResolution().y);
    shadowFb = renderer.makeFrameBuffer(FrameBufferProperties{
        .depthFormat = ImageFormat::DEPTH,
        .wrapping = TextureWrapping::CLAMP_BORDER
    }, 512, 512);

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
        .withSampler2D(ppShader->samplerLocation("frameBuffer").value(), ppSampler)
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
    testFb.clear({0, 0, 0, 0});

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

    //postProcessing();

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
    dirLightQuery.each([&] (const core::GlobalTransform3D& transform, const DirectionalLight3D& light) {
        pointLights.emplace_back(MeshLight{
            .pos = transform.transform.position(),
            .dir = transform.transform.rotation(),
            .color = light.color,
            .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
            .brightness = light.brightness,
            .type = LightType::Directional,
            .castShadows = light.castShadows
        });
    });

    pointLightQuery.each([&] (const core::GlobalTransform3D& transform, const PointLight3D& light) {
        pointLights.emplace_back(MeshLight{
            .pos = transform.transform.position(),
            .color = light.color,
            .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
            .brightness = light.brightness,
            .type = LightType::Point,
            .castShadows = light.castShadows
        });
    });

    spotLightQuery.each([&] (const core::GlobalTransform3D& transform, const SpotLight3D& light) {
        pointLights.emplace_back(MeshLight{
            .pos = transform.transform.position(),
            .dir = transform.transform.rotation(),
            .color = light.color,
            .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
            .brightness = light.brightness,
            .outer = light.outerAngle,
            .inner = light.innerAngle,
            .type = LightType::Spot,
            .castShadows = light.castShadows
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

        //pipeline.renderInstanced(testFb, instance.numInstances, instance.mesh->numVertices());
        pipeline.renderInstanced(instance.numInstances, instance.mesh->numVertices());
    }
}

void MeshRenderLayer::renderLight (const MeshLight& light) {
    bpLight->lightSpace = getLightSpaceMatrix(light);
    bpLight->lightPos = light.pos;
    bpLight->lightDir = light.dir * core::Quaternion::ForwardVector * -1.0f;
    bpLight->lightColor = light.color;
    bpLight->ambientColor = light.ambientColor / static_cast<float>(pointLights.size());
    bpLight->brightness = light.brightness;
    bpLight->cosInner = glm::cos(light.inner);
    bpLight->cosOuter = glm::cos(light.outer);
    bpLight->lightType = static_cast<int>(light.type);
    bpLight->castShadows = light.castShadows;
    bpLight.upload();

    // if (light.castShadows) {
    //     renderShadowMap(light);
    // }

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

        // if (light.castShadows) {
        //     pipeline.bindSampler(matPipeline.shadowMapBinding, shadowFb.depthSampler());
        // }

        matInstance->bind(matPipeline);

        //pipeline.renderInstanced(testFb, instance.numInstances, instance.mesh->numVertices());
        pipeline.renderInstanced(instance.numInstances, instance.mesh->numVertices());
    }
}

glm::mat4 MeshRenderLayer::getLightSpaceMatrix (const MeshLight& light) {
    return getLightSpaceProj(light) * getLightSpaceView(light);
}

glm::mat4 MeshRenderLayer::getLightSpaceView (const MeshLight& light) {
    if (light.type == LightType::Directional || light.type == LightType::Spot) {
        if (light.type == LightType::Directional) {
            //view = glm::identity<glm::mat4>();
            glm::mat4 translationMatrix{
                        {1, 0, 0, 0},
                        {0, 1, 0, 0},
                        {0, 0, 1, 0},
                        {-light.pos.x, -light.pos.y, -light.pos.z, 1}
            };
            glm::mat4 fixRotation = {
                {1, 0, 0, 0},
                {0, -1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            };
            return fixRotation * static_cast<glm::mat4>(light.dir.inverse()) * translationMatrix;
        } else {
            glm::mat4 translationMatrix{
                    {1, 0, 0, 0},
                    {0, 1, 0, 0},
                    {0, 0, 1, 0},
                    {-light.pos.x, -light.pos.y, -light.pos.z, 1}
            };
            glm::mat4 fixRotation = {
                {1, 0, 0, 0},
                {0, -1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}
            };
            return fixRotation * static_cast<glm::mat4>(light.dir.inverse()) * translationMatrix;
        }
    } else {
        return glm::identity<glm::mat4>();
    }
}

glm::mat4 MeshRenderLayer::getLightSpaceProj (const MeshLight& light) {
    if (light.type == LightType::Directional || light.type == LightType::Spot) {
        if (light.type == LightType::Directional) {
            return glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.001f, 20.0f);
        } else {
            return glm::perspective(light.outer, 1.0f, 0.1f, 100.0f);
        }
    } else {
        return glm::identity<glm::mat4>();
    }
}


void MeshRenderLayer::renderShadowMap (const MeshLight& light) {
    shadowFb.clear();
    if (light.type == LightType::Directional || light.type == LightType::Spot) {
        for (const auto& instance : instances) {
            auto& smPipeline = instance.materialInstance->material()->getShadowMapPipeline(instance.mesh->layout());

            auto& pipeline = smPipeline.pipeline;
            pipeline.bindUniform(smPipeline.lightUniform, bpLight);

            auto& streams = instance.mesh->streams();
            PHENYL_DASSERT(streams.size() == smPipeline.streamBindings.size());
            for (std::size_t i = 0; i < streams.size(); i++) {
                pipeline.bindBuffer(smPipeline.streamBindings[i], streams[i]);
            }

            pipeline.bindBuffer(smPipeline.modelBinding, instanceBuffer, instance.instanceOffset);
            pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

            pipeline.renderInstanced(shadowFb, instance.numInstances, instance.mesh->numVertices());
        }
    } else {
        // TODO
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