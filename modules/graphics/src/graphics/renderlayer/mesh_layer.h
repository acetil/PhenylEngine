#pragma once

#include "core/runtime.h"
#include "core/world.h"
#include "core/components/3d/global_transform.h"
#include "util/map.h"

#include "graphics/mesh/mesh.h"
#include "graphics/pipeline.h"
#include "graphics/abstract_render_layer.h"
#include "graphics/camera_3d.h"
#include "graphics/components/3d/mesh_renderer.h"

namespace phenyl::graphics {
    class MeshRenderLayer : public AbstractRenderLayer {
    private:
        struct GlobalUniform {
            glm::mat4 view;
            glm::mat4 projection;
        };

        struct MeshRenderRequest {
            std::uint64_t layout;
            const Mesh* mesh;
            glm::mat4 transform;
        };

        struct MeshInstances {
            const Mesh* mesh;
            std::size_t instanceOffset;
            std::size_t numInstances;
        };

        struct MeshPipeline {
            Pipeline pipeline;
            UniformBinding globalUniform;
            BufferBinding instanceBinding;
            std::vector<BufferBinding> streamBindings;
        };

        Renderer* renderer = nullptr;
        core::Query<core::GlobalTransform3D, MeshRenderer3D> meshQuery;
        util::HashMap<std::uint64_t, MeshPipeline> pipelines; // TODO
        Buffer<glm::mat4> instanceBuffer; // TODO: per material
        UniformBuffer<GlobalUniform> globalUniform{};

        std::vector<MeshRenderRequest> requests;
        std::vector<MeshInstances> instances;

        MeshPipeline& getPipeline (const MeshLayout& layout);
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
