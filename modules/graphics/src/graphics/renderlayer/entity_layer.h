#pragma once

#include "core/world.h"
#include "graphics/backend/abstract_render_layer.h"
#include "graphics/backend/buffer.h"
#include "graphics/backend/pipeline.h"
#include "graphics/camera_2d.h"

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
        ISampler* sampler;
    };

    EntityRenderLayer ();

    [[nodiscard]] std::string_view getName () const override;

    void init (Renderer& renderer) override;

    void render () override;

    void pushEntity (const core::GlobalTransform2D& transform, const Sprite2D& sprite);
    void bufferEntities (const Camera2D& camera);

    void addSystems (core::PhenylRuntime& runtime);

private:
    struct Uniform {
        glm::mat4 camera;
    };

    std::vector<std::pair<ISampler*, std::uint16_t>> m_samplerStartIndices;
    std::vector<SamplerRender> m_samplerRenders;

    Pipeline m_pipeline;

    Buffer<Vertex> m_vertexBuffer;
    Buffer<std::uint16_t> m_indices;

    UniformBinding m_uniformBinding{};
    UniformBuffer<Uniform> m_uniformBuffer;

    SamplerBinding m_samplerBinding{};
};
} // namespace phenyl::graphics
