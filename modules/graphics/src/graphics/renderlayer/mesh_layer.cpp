#include "mesh_layer.h"

#include "core/assets/assets.h"

#include <graphics/backend/renderer.h>

using namespace phenyl::graphics;

MeshRenderLayer::MeshRenderLayer (core::World& world) :
    AbstractRenderLayer{0},
    m_meshQuery{world.query<core::GlobalTransform3D, MeshRenderer3D>()},
    m_pointLightQuery{world.query<core::GlobalTransform3D, PointLight3D>()},
    m_dirLightQuery{world.query<core::GlobalTransform3D, DirectionalLight3D>()},
    m_spotLightQuery{world.query<core::GlobalTransform3D, SpotLight3D>()} {}

std::string_view MeshRenderLayer::getName () const {
    return "MeshRenderLayer";
}

void MeshRenderLayer::init (Renderer& renderer) {
    this->m_renderer = &renderer;
    auto& viewport = renderer.getViewport();

    m_testFb = renderer.makeFrameBuffer(
        FrameBufferProperties{
          .format = ImageFormat::RGBA,
          .depthFormat = ImageFormat::DEPTH24_STENCIL8,
        },
        viewport.getResolution().x, viewport.getResolution().y);
    m_shadowFb = renderer.makeFrameBuffer(
        FrameBufferProperties{
          .depthFormat = ImageFormat::DEPTH,
          .wrapping = TextureWrapping::CLAMP_BORDER,
          .borderColor = TextureBorderColor::WHITE,
        },
        512, 512);

    m_instanceBuffer = renderer.makeBuffer<glm::mat4>(512, BufferStorageHint::DYNAMIC);
    m_globalUniform = renderer.makeUniformBuffer<MeshGlobalUniform>();
    m_bpLights = renderer.makeUniformArrayBuffer<BPLightUniform>();

    // meshMaterial = core::Assets::Load<Material>("resources/phenyl/materials/blinn_phong");

    auto ppShader = core::Assets::Load2<Shader>("phenyl/shaders/postprocess/noop");
    BufferBinding vertexBinding;

    m_postProcessPipeline = renderer.buildPipeline()
                                .withShader(ppShader)
                                .withBuffer<glm::vec2>(vertexBinding)
                                .withAttrib<glm::vec2>(0, vertexBinding)
                                .withSampler2D(ppShader->samplerLocation("frameBuffer").value(), m_ppSampler)
                                .build();

    m_ppQuad = renderer.makeBuffer<glm::vec2>(6, BufferStorageHint::STATIC);
    m_ppQuad.emplace(-1, -1);
    m_ppQuad.emplace(1, -1);
    m_ppQuad.emplace(1, 1);

    m_ppQuad.emplace(1, 1);
    m_ppQuad.emplace(-1, 1);
    m_ppQuad.emplace(-1, -1);

    m_ppQuad.upload();

    m_postProcessPipeline.bindBuffer(vertexBinding, m_ppQuad);
}

void MeshRenderLayer::addSystems (core::PhenylRuntime& runtime) {
    runtime.addSystem<core::Render>("MeshRenderLayer::upload", this, &MeshRenderLayer::uploadSystem);
}

void MeshRenderLayer::uploadSystem (core::PhenylRuntime& runtime) {
    uploadData(runtime.resource<Camera3D>());
}

void MeshRenderLayer::uploadData (Camera3D& camera) {
    m_testFb.clear({0, 0, 0, 0});

    gatherGeometry();
    gatherLights();

    m_globalUniform->view = camera.view();
    m_globalUniform->projection = camera.projection();
    m_globalUniform->viewPos = camera.transform.position();
    m_globalUniform.upload();
}

void MeshRenderLayer::render (Renderer& renderer) {
    PHENYL_DASSERT(m_renderer);
    auto cmdList = renderer.getCommandList();

    depthPrepass(cmdList);

    bufferLights();
    for (std::size_t i = 0; i < m_pointLights.size(); i++) {
        renderLight(cmdList, m_pointLights[i], i);
    }

    postProcessing(cmdList);

    m_instances.clear();
    m_pointLights.clear();

    m_instanceBuffer.clear();
}

void MeshRenderLayer::gatherGeometry () {
    m_meshQuery.each([&] (const core::GlobalTransform3D& transform, MeshRenderer3D& renderer) {
        auto* mesh = renderer.mesh.get();
        auto* matInstance = renderer.material.get();
        PHENYL_DASSERT(mesh);
        // requests.emplace_back(mesh->layout().layoutId, mesh,
        // transform.transform.transformMatrx());
        m_requests.emplace_back(MeshRenderRequest{
          .layout = mesh->layout().layoutId,
          .mesh = mesh,
          .materialInstance = matInstance,
          .transform = transform.transform,
        });
    });

    std::ranges::sort(m_requests, [] (const MeshRenderRequest& lhs, const MeshRenderRequest& rhs) {
        if (lhs.materialInstance->material() != rhs.materialInstance->material()) {
            return lhs.materialInstance->material() < rhs.materialInstance->material();
        }

        if (lhs.materialInstance != rhs.materialInstance) {
            return lhs.materialInstance < rhs.materialInstance;
        }

        return lhs.layout != rhs.layout ? lhs.layout < rhs.layout : lhs.mesh < rhs.mesh;
    });

    for (std::size_t i = 0; i < m_requests.size(); i++) {
        auto& req = m_requests[i];
        m_instanceBuffer.emplace(req.transform);

        if (m_instances.empty() || m_instances.back().mesh != req.mesh ||
            m_instances.back().materialInstance != req.materialInstance) {
            m_instances.emplace_back(MeshInstances{
              .mesh = req.mesh,
              .materialInstance = req.materialInstance,
              .instanceOffset = i,
              .numInstances = 1,
            });
        } else {
            m_instances.back().numInstances++;
        }
    }
    m_instanceBuffer.upload();
    m_requests.clear();
}

void MeshRenderLayer::gatherLights () {
    m_dirLightQuery.each([&] (const core::GlobalTransform3D& transform, const DirectionalLight3D& light) {
        m_pointLights.emplace_back(MeshLight{
          .pos = transform.position(),
          .dir = transform.rotation(),
          .color = light.color,
          .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
          .brightness = light.brightness,
          .type = LightType::Directional,
          .castShadows = light.castShadows,
        });
    });

    m_pointLightQuery.each([&] (const core::GlobalTransform3D& transform, const PointLight3D& light) {
        m_pointLights.emplace_back(MeshLight{
          .pos = transform.position(),
          .color = light.color,
          .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
          .brightness = light.brightness,
          .type = LightType::Point,
          .castShadows = light.castShadows,
        });
    });

    m_spotLightQuery.each([&] (const core::GlobalTransform3D& transform, const SpotLight3D& light) {
        m_pointLights.emplace_back(MeshLight{
          .pos = transform.position(),
          .dir = transform.rotation(),
          .color = light.color,
          .ambientColor = glm::vec3{1.0f, 1.0f, 1.0f},
          .brightness = light.brightness,
          .outer = light.outerAngle,
          .inner = light.innerAngle,
          .type = LightType::Spot,
          .castShadows = light.castShadows,
        });
    });
}

void MeshRenderLayer::depthPrepass (CommandList& cmdList) {
    for (const auto& instance : m_instances) {
        auto& depthPipeline = instance.materialInstance->material()->getDepthPipeline(instance.mesh->layout());

        auto& pipeline = depthPipeline.pipeline;
        pipeline.bindUniform(depthPipeline.globalUniform, m_globalUniform);

        auto& streams = instance.mesh->streams();
        PHENYL_DASSERT(streams.size() == depthPipeline.streamBindings.size());
        for (std::size_t i = 0; i < streams.size(); i++) {
            pipeline.bindBuffer(depthPipeline.streamBindings[i], streams[i]);
        }

        pipeline.bindBuffer(depthPipeline.modelBinding, m_instanceBuffer, instance.instanceOffset);
        pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

        pipeline.renderInstanced(cmdList, m_testFb, instance.numInstances, instance.mesh->numVertices());
        // pipeline.renderInstanced(instance.numInstances, instance.mesh->numVertices());
    }
}

void MeshRenderLayer::bufferLights () {
    m_bpLights.clear();
    m_bpLights.reserve(m_pointLights.size());
    for (const auto& light : m_pointLights) {
        m_bpLights.push(BPLightUniform{
          .lightSpace = getLightSpaceMatrix(light),
          .lightPos = light.pos,
          .lightDir = light.dir * core::Quaternion::ForwardVector * -1.0f,
          .lightColor = light.color,
          .ambientColor = light.ambientColor / static_cast<float>(m_pointLights.size()),
          .brightness = light.brightness,
          .cosOuter = glm::cos(light.outer),
          .cosInner = glm::cos(light.inner),
          .lightType = static_cast<int>(light.type),
          .castShadows = light.castShadows,
        });
    }

    m_bpLights.upload();
}

void MeshRenderLayer::renderLight (CommandList& cmdList, const MeshLight& light, std::size_t index) {
    if (light.castShadows) {
        renderShadowMap(cmdList, light, index);
    }

    for (const auto& instance : m_instances) {
        // auto& meshPipeline = getPipeline(instance.mesh->layout()); // TODO: ahead of time
        // pipeline creation
        auto* matInstance = instance.materialInstance;
        auto& matPipeline = matInstance->material()->getPipeline(instance.mesh->layout());
        auto& pipeline = matPipeline.pipeline;

        pipeline.bindUniform(matPipeline.globalUniform, m_globalUniform);
        pipeline.bindUniform(matPipeline.lightUniform, m_bpLights, index);

        auto& streams = instance.mesh->streams();
        PHENYL_DASSERT(streams.size() == matPipeline.streamBindings.size());
        for (std::size_t i = 0; i < streams.size(); i++) {
            pipeline.bindBuffer(matPipeline.streamBindings[i], streams[i]);
        }

        pipeline.bindBuffer(matPipeline.modelBinding, m_instanceBuffer, instance.instanceOffset);
        pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

        if (light.castShadows) {
            pipeline.bindSampler(matPipeline.shadowMapBinding, m_shadowFb.depthSampler());
        }

        matInstance->bind(matPipeline);

        pipeline.renderInstanced(cmdList, m_testFb, instance.numInstances, instance.mesh->numVertices());
        // pipeline.renderInstanced(instance.numInstances, instance.mesh->numVertices());
    }
}

glm::mat4 MeshRenderLayer::getLightSpaceMatrix (const MeshLight& light) {
    return getLightSpaceProj(light) * getLightSpaceView(light);
}

glm::mat4 MeshRenderLayer::getLightSpaceView (const MeshLight& light) {
    if (light.type == LightType::Directional || light.type == LightType::Spot) {
        if (light.type == LightType::Directional) {
            // view = glm::identity<glm::mat4>();
            glm::mat4 translationMatrix{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0},
              {-light.pos.x, -light.pos.y, -light.pos.z, 1}};
            glm::mat4 fixRotation = {{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
            return fixRotation * static_cast<glm::mat4>(light.dir.inverse()) * translationMatrix;
        } else {
            glm::mat4 translationMatrix{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0},
              {-light.pos.x, -light.pos.y, -light.pos.z, 1}};
            glm::mat4 fixRotation = {{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
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
            return glm::perspective(light.outer * 2, 1.0f, 1.0f, 100.0f);
        }
    } else {
        return glm::identity<glm::mat4>();
    }
}

void MeshRenderLayer::renderShadowMap (CommandList& cmdList, const MeshLight& light, std::size_t index) {
    m_shadowFb.clear();
    if (light.type == LightType::Directional || light.type == LightType::Spot) {
        for (const auto& instance : m_instances) {
            auto& smPipeline = instance.materialInstance->material()->getShadowMapPipeline(instance.mesh->layout());

            auto& pipeline = smPipeline.pipeline;
            pipeline.bindUniform(smPipeline.lightUniform, m_bpLights, index);

            auto& streams = instance.mesh->streams();
            PHENYL_DASSERT(streams.size() == smPipeline.streamBindings.size());
            for (std::size_t i = 0; i < streams.size(); i++) {
                pipeline.bindBuffer(smPipeline.streamBindings[i], streams[i]);
            }

            pipeline.bindBuffer(smPipeline.modelBinding, m_instanceBuffer, instance.instanceOffset);
            pipeline.bindIndexBuffer(instance.mesh->layout().indexType, instance.mesh->indices());

            pipeline.renderInstanced(cmdList, m_shadowFb, instance.numInstances, instance.mesh->numVertices());
        }
    } else {
        // TODO
    }
}

void MeshRenderLayer::postProcessing (CommandList& cmdList) {
    m_postProcessPipeline.bindSampler(m_ppSampler, m_testFb.sampler());
    m_postProcessPipeline.render(cmdList, 6);
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
//         .withUniform<GlobalUniform>(*shader->uniformLocation("GlobalUniform"),
//         globalUniform);
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
