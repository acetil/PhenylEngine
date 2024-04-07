#pragma once

#include "graphics/abstract_render_layer.h"
#include "graphics/maths_headers.h"
#include "graphics/renderer.h"
#include "graphics/font/font.h"

namespace phenyl::graphics {
    class CanvasRenderLayer : public AbstractRenderLayer, public IGlyphRenderer {
    private:
        struct Uniform {
            glm::vec2 screenSize;
        };

        struct Vertex {
            glm::vec2 pos;
            glm::vec3 uv;
            glm::vec4 colour;
        };

        Pipeline pipeline;
        GlyphAtlas& glyphAtlas;
        SamplerBinding samplerBinding{};
        Buffer<Vertex> buffer;
        Buffer<std::uint16_t> indices;
        UniformBinding uniformBinding;
        UniformBuffer<Uniform> uniformBuffer;

        void uploadData ();
    public:
        explicit CanvasRenderLayer (GlyphAtlas& glyphAtlas);

        [[nodiscard]] std::string_view getName () const override;
        void init (Renderer& renderer) override;
        void render () override;

        void setScreenSize (glm::vec2 screenSize);

        void renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) override;
        void renderConvexPoly (std::span<glm::vec2> points, glm::vec4 colour);
        void renderConvexPolyAA (std::span<glm::vec2> points, glm::vec4 colour, float widthAA=1.0f);
        void renderPolyLine (std::span<glm::vec2> points, glm::vec4 colour, float width, bool closed);
        void renderPolyLineAA (std::span<glm::vec2> points, glm::vec4 colour, float width, bool closed, float widthAA=1.0f);
    };
}
