#pragma once

#include "core/runtime.h"
#include "core/world.h"
#include "core/components/3d/global_transform.h"
#include "util/map.h"

#include "graphics/mesh/mesh.h"
#include "graphics/pipeline.h"
#include "graphics/abstract_render_layer.h"
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
        alignas(16) glm::vec3 lightPos;
        alignas(16) glm::vec3 lightColor;
        alignas(16) glm::vec3 ambientColor;
        float brightness;
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

        struct MeshLight {
            glm::vec3 pos;
            glm::vec3 color;
            float brightness;
            glm::vec3 ambientColor;
        };

        Renderer* renderer = nullptr;
        //core::Asset<Material> meshMaterial; // TODO
        core::Query<core::GlobalTransform3D, MeshRenderer3D> meshQuery;
        core::Query<core::GlobalTransform3D, PointLight3D> pointLightQuery;

        //util::HashMap<std::uint64_t, MeshPipeline> pipelines; // TODO
        Buffer<glm::mat4> instanceBuffer; // TODO: per material
        UniformBuffer<MeshGlobalUniform> globalUniform{};
        UniformBuffer<BPLightUniform> bpLight;
        std::vector<MeshLight> pointLights;

        std::vector<MeshRenderRequest> requests;
        std::vector<MeshInstances> instances;

        //MeshPipeline& getPipeline (const MeshLayout& layout);
        void gatherGeometry ();
        void gatherLights ();

        void depthPrepass ();
        void renderLight (const MeshLight& light);
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
