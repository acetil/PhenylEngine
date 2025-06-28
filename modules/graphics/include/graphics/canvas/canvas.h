#pragma once

#include "canvas_shapes.h"
#include "canvas_style.h"
#include "graphics/font/glyph_atlas.h"
#include "graphics/viewport.h"

namespace phenyl::graphics {
class Font;
class FontManager;
class Renderer;
class CanvasRenderLayer;

class Canvas : public core::IResource, private IViewportUpdateHandler {
public:
    explicit Canvas (Renderer& renderer);
    ~Canvas () override;

    void render (glm::vec2 pos, const CanvasRect& rect, const CanvasStyle& style);
    void render (glm::vec2 pos, const CanvasRoundedRect& rect, const CanvasStyle& style);

    void render (const CanvasRect& rect, const CanvasStyle& style) {
        render({0, 0}, rect, style);
    }

    void render (const CanvasRoundedRect& rect, const CanvasStyle& style) {
        render({0, 0}, rect, style);
    }

    void renderText (glm::vec2 pos, core::Asset<Font>& font, std::uint32_t size, std::string_view text,
        glm::vec3 colour = {1.0f, 1.0f, 1.0f});

    void renderPolygon (std::span<const glm::vec2> vertices, const CanvasStyle& style);

    core::Asset<Font>& defaultFont () noexcept {
        return m_defaultFont;
    }

    void pushOffset (glm::vec2 off);
    void popOffset ();

    std::string_view getName () const noexcept override;

    glm::ivec2 resolution () const noexcept {
        return m_resolution;
    }

private:
    GlyphAtlas m_atlas;
    CanvasRenderLayer& m_layer;
    std::unique_ptr<FontManager> m_fontManager;
    core::Asset<Font> m_defaultFont;

    std::vector<glm::vec2> m_offsetStack;
    glm::ivec2 m_resolution;

    void submitVertices (std::span<const glm::vec2> vertices, const CanvasStyle& style);
    [[nodiscard]] glm::vec2 offset () const;

    void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
};
} // namespace phenyl::graphics
