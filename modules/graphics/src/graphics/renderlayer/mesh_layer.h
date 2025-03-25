#pragma once

#include "core/runtime.h"
#include "core/world.h"
#include "core/components/3d/global_transform.h"
#include "util/map.h"

#include "graphics/mesh/mesh.h"
#include "graphics/backend/pipeline.h"
#include "graphics/backend/abstract_render_layer.h"
#include "graphics/camera_3d.h"
#include "graphics/material.h"
#include "graphics/components/3d/mesh_renderer.h"
#include "graphics/components/3d/lighting.h"

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

        Renderer* renderer = nullptr;
        //core::Asset<Material> meshMaterial; // TODO
        core::Query<core::GlobalTransform3D, MeshRenderer3D> meshQuery;
        core::Query<core::GlobalTransform3D, PointLight3D> pointLightQuery;
        core::Query<core::GlobalTransform3D, DirectionalLight3D> dirLightQuery;
        core::Query<core::GlobalTransform3D, SpotLight3D> spotLightQuery;

        //util::HashMap<std::uint64_t, MeshPipeline> pipelines; // TODO
        FrameBuffer testFb;
        FrameBuffer shadowFb;
        Buffer<glm::mat4> instanceBuffer; // TODO: per material
        UniformBuffer<MeshGlobalUniform> globalUniform{};
        UniformBuffer<BPLightUniform> bpLight;
        std::vector<MeshLight> pointLights;

        std::vector<MeshRenderRequest> requests;
        std::vector<MeshInstances> instances;

        Pipeline postProcessPipeline;
        SamplerBinding ppSampler;
        Buffer<glm::vec2> ppQuad;

        //MeshPipeline& getPipeline (const MeshLayout& layout);
        void gatherGeometry ();
        void gatherLights ();

        void depthPrepass ();
        void renderLight (const MeshLight& light);
        glm::mat4 getLightSpaceView (const MeshLight& light);
        glm::mat4 getLightSpaceProj (const MeshLight& light);
        glm::mat4 getLightSpaceMatrix (const MeshLight& light);
        void renderShadowMap (const MeshLight& light);
        void postProcessing ();
    public:
        explicit MeshRenderLayer (core::World& world);

        std::string_view getName() const override;

        void init (Renderer& renderer) override;
        void addSystems (core::PhenylRuntime& runtime);
        void uploadData (Camera3D& camera);
        void uploadSystem (core::PhenylRuntime& runtime);
        void render () override;
    };
}
