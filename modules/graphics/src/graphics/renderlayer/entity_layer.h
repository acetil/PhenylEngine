#pragma once

#include "core/world.h"

#include "graphics/abstract_render_layer.h"
#include "graphics/camera_2d.h"
#include "graphics/buffer.h"
#include "graphics/pipeline.h"

namespace phenyl::core {
    struct GlobalTransform2D;
}

namespace phenyl::core {
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

        void bufferData (core::World& world, const Camera2D& camera);
    public:

        EntityRenderLayer ();

        [[nodiscard]] std::string_view getName () const override;

        void init (Renderer& renderer) override;

        void preRender (core::World& world, const Camera2D& camera);
        void render () override;

        void pushEntity (const core::GlobalTransform2D& transform, const Sprite2D& sprite);
        void bufferEntities (const Camera2D& camera);

        void addSystems (core::PhenylRuntime& runtime);
    };
}
