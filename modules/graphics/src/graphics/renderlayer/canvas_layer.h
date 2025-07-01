#pragma once

#include "graphics/backend/abstract_render_layer.h"
#include "graphics/backend/renderer.h"
#include "graphics/font/font.h"
#include "graphics/maths_headers.h"

namespace phenyl::graphics {
class CanvasRenderLayer : public AbstractRenderLayer, public IGlyphRenderer {
public:
    explicit CanvasRenderLayer (GlyphAtlas& glyphAtlas);

    [[nodiscard]] std::string_view getName () const override;
    void init (Renderer& renderer) override;
    void render (Renderer& renderer) override;

    void setScreenSize (glm::vec2 screenSize);

    void renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) override;
    void renderConvexPoly (std::span<const glm::vec2> points, glm::vec4 colour);
    void renderConvexPolyAA (std::span<const glm::vec2> points, glm::vec4 colour, float widthAA = 1.0f);
    void renderPolyLine (std::span<const glm::vec2> points, glm::vec4 colour, float width, bool closed);
    void renderPolyLineAA (std::span<const glm::vec2> points, glm::vec4 colour, float width, bool closed,
        float widthAA = 1.0f);

private:
    struct Uniform {
        glm::vec2 screenSize;
    };

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 uv;
        glm::vec4 colour;
    };

    Pipeline m_pipeline;
    GlyphAtlas& m_glyphAtlas;
    SamplerBinding m_samplerBinding{};
    Buffer<Vertex> m_buffer;
    Buffer<std::uint16_t> m_indices;
    UniformBinding m_uniformBinding;
    UniformBuffer<Uniform> m_uniformBuffer;

    void uploadData ();
};
} // namespace phenyl::graphics
