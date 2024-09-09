#pragma once

#include "graphics/viewport.h"
#include "graphics/font/glyph_atlas.h"

#include "canvas_shapes.h"
#include "canvas_style.h"

namespace phenyl::graphics {
    class Font;
    class FontManager;
    class Renderer;
    class CanvasRenderLayer;

    class Canvas : public runtime::IResource, private IViewportUpdateHandler {
    private:
        GlyphAtlas atlas;
        CanvasRenderLayer& layer;
        std::unique_ptr<FontManager> fontManager;
        common::Asset<Font> defaultFontAsset;

        std::vector<glm::vec2> offsetStack;
        glm::ivec2 canvasResolution;

        void submitVertices (std::span<glm::vec2> vertices, const CanvasStyle& style);
        [[nodiscard]] glm::vec2 offset () const;

        void onViewportResize(glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
    public:
        explicit Canvas (Renderer& renderer);
        ~Canvas() override;

        void render (glm::vec2 pos, const CanvasRect& rect, const CanvasStyle& style);
        void render (glm::vec2 pos, const CanvasRoundedRect& rect, const CanvasStyle& style);

        void render (const CanvasRect& rect, const CanvasStyle& style) {
            render({0, 0}, rect, style);
        }
        void render (const CanvasRoundedRect& rect, const CanvasStyle& style) {
            render({0, 0}, rect, style);
        }

        void renderText (glm::vec2 pos, common::Asset<Font>& font, std::uint32_t size, std::string_view text, glm::vec3 colour = {1.0f, 1.0f, 1.0f});

        common::Asset<Font>& defaultFont () noexcept {
            return defaultFontAsset;
        }

        void pushOffset (glm::vec2 off);
        void popOffset ();

        std::string_view getName () const noexcept override;

        glm::ivec2 resolution () const noexcept {
            return canvasResolution;
        }
    };
}
