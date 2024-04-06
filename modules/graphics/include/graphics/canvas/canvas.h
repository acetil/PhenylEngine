#pragma once

#include "graphics/font/glyph_atlas.h"

#include "canvas_shapes.h"
#include "canvas_style.h"

namespace phenyl::graphics {
    class Font;
    class FontManager;
    class Renderer;
    class CanvasRenderLayer;

    class Canvas : public runtime::IResource {
    private:
        GlyphAtlas atlas;
        CanvasRenderLayer& layer;
        std::unique_ptr<FontManager> fontManager;
        glm::vec2 screenSize;

        std::vector<glm::vec2> offsetStack;

        void submitVertices (std::span<glm::vec2> vertices, const CanvasStyle& style);
        [[nodiscard]] glm::vec2 offset () const;
    public:
        explicit Canvas (Renderer& renderer);
        ~Canvas();

        void render (glm::vec2 pos, const CanvasRect& rect, const CanvasStyle& style);
        void render (glm::vec2 pos, const CanvasRoundedRect& rect, const CanvasStyle& style);

        void render (const CanvasRect& rect, const CanvasStyle& style) {
            render({0, 0}, rect, style);
        }
        void render (const CanvasRoundedRect& rect, const CanvasStyle& style) {
            render({0, 0}, rect, style);
        }

        void renderText (glm::vec2 pos, common::Asset<Font>& font, std::uint32_t size, std::string_view text, glm::vec3 colour = {1.0f, 1.0f, 1.0f});

        void pushOffset (glm::vec2 off);
        void popOffset ();

        std::string_view getName () const noexcept override;;
    };
}
