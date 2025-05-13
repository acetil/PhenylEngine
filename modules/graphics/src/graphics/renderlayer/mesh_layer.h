#pragma once

#include "core/components/3d/global_transform.h"
#include "core/runtime.h"
#include "core/world.h"
#include "graphics/backend/abstract_render_layer.h"
#include "graphics/backend/pipeline.h"
#include "graphics/camera_3d.h"
#include "graphics/components/3d/lighting.h"
#include "graphics/components/3d/mesh_renderer.h"
#include "graphics/material.h"
#include "graphics/mesh/mesh.h"
#include "util/map.h"

namespace phenyl::graphics {
struct MeshGlobalUniform {
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 viewPos;
};

struct BPLightUniform {
    // std140 alignment
    alignas(16) glm::mat4 lightSpace;
    alignas(16) glm::vec3 lightPos;
    alignas(16) glm::vec3 lightDir;
    alignas(16) glm::vec3 lightColor;
    alignas(16) glm::vec3 ambientColor;
    float brightness;
    float cosOuter;
    float cosInner;
    int lightType = 0;
    int castShadows; // TODO: combine with light type
};

class MeshRenderLayer : public AbstractRenderLayer {
public:
    explicit MeshRenderLayer (core::World& world);

    std::string_view getName () const override;

    void init (Renderer& renderer) override;
    void addSystems (core::PhenylRuntime& runtime);
    void uploadData (Camera3D& camera);
    void uploadSystem (core::PhenylRuntime& runtime);
    void render () override;

private:
    struct MeshRenderRequest {
        std::uint64_t layout;
        const Mesh* mesh;
        MaterialInstance* materialInstance;
        glm::mat4 transform;
    };

    struct MeshInstances {
        const Mesh* mesh;
        MaterialInstance* materialInstance;
        std::size_t instanceOffset;
        std::size_t numInstances;
    };

    struct MeshPipeline {
        Pipeline pipeline;
        UniformBinding globalUniform;
        BufferBinding instanceBinding;
        std::vector<BufferBinding> streamBindings;
    };

    enum class LightType {
        Point = 0,
        Directional = 1,
        Spot = 2
    };

    struct MeshLight {
        glm::vec3 pos;
        core::Quaternion dir;
        glm::vec3 color;
        glm::vec3 ambientColor;
        float brightness;
        float outer;
        float inner;
        LightType type;
        bool castShadows;
    };

    Renderer* m_renderer = nullptr;
    core::Query<core::GlobalTransform3D, MeshRenderer3D> m_meshQuery;
    core::Query<core::GlobalTransform3D, PointLight3D> m_pointLightQuery;
    core::Query<core::GlobalTransform3D, DirectionalLight3D> m_dirLightQuery;
    core::Query<core::GlobalTransform3D, SpotLight3D> m_spotLightQuery;

    FrameBuffer m_testFb;
    FrameBuffer m_shadowFb;
    Buffer<glm::mat4> m_instanceBuffer; // TODO: per material
    UniformBuffer<MeshGlobalUniform> m_globalUniform{};
    UniformArrayBuffer<BPLightUniform> m_bpLights;
    std::vector<MeshLight> m_pointLights;

    std::vector<MeshRenderRequest> m_requests;
    std::vector<MeshInstances> m_instances;

    Pipeline m_postProcessPipeline;
    SamplerBinding m_ppSampler;
    Buffer<glm::vec2> m_ppQuad;

    void gatherGeometry ();
    void gatherLights ();

    void depthPrepass ();
    void bufferLights ();
    void renderLight (const MeshLight& light, std::size_t lightIndex);
    glm::mat4 getLightSpaceView (const MeshLight& light);
    glm::mat4 getLightSpaceProj (const MeshLight& light);
    glm::mat4 getLightSpaceMatrix (const MeshLight& light);
    void renderShadowMap (const MeshLight& light, std::size_t index);
    void postProcessing ();
};
} // namespace phenyl::graphics
