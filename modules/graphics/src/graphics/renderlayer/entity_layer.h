#pragma once

#include "component/component.h"

#include "graphics/abstract_render_layer.h"
#include "graphics/camera.h"
#include "graphics/buffer.h"
#include "graphics/pipeline.h"

namespace phenyl::common {
    struct GlobalTransform2D;
}

namespace phenyl::runtime {
    class PhenylRuntime;
}

namespace phenyl::graphics {
    struct Sprite2D;

    class EntityRenderLayer : public AbstractRenderLayer {
    public:
        struct Vertex {
            glm::vec2 pos;
            glm::vec2 uv;
        };

        struct SamplerRender {
            std::uint16_t indexOffset;
            std::uint16_t size;
            const ISampler* sampler;
        };
    private:
        struct Uniform {
            glm::mat4 camera;
        };

        std::vector<std::pair<const ISampler*, std::uint16_t>> samplerStartIndices;
        std::vector<SamplerRender> samplerRenders;

        Pipeline pipeline;

        //Buffer<glm::vec2> posBuffer;
       // Buffer<glm::vec2> uvBuffer;
        Buffer<Vertex> vertexBuffer;
        Buffer<std::uint16_t> indices;

        UniformBinding uniformBinding{};
        UniformBuffer<Uniform> uniformBuffer;

        SamplerBinding samplerBinding{};

        void bufferData (component::World& world, const Camera& camera);
    public:

        EntityRenderLayer ();

        [[nodiscard]] std::string_view getName () const override;

        void init (Renderer& renderer) override;

        void preRender (component::World& world, const Camera& camera);
        void render () override;

        void pushEntity (const common::GlobalTransform2D& transform, const Sprite2D& sprite);
        void bufferEntities (const Camera& camera);

        void addSystems (runtime::PhenylRuntime& runtime);
    };
}
