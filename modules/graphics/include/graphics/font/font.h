#pragma once

#include <string>

#include "font_face.h"
#include "graphics/font/glyph_atlas.h"
#include "graphics/font/rendered_text.h"

#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    class Font {
    private:
        FontFace face;
        GlyphAtlas atlas;
        int screenX = 800;
        int screenY = 600;
    public:
        Font (FontFace _face, int targetRes) : atlas(face.getGlyphs(), targetRes), face(std::move(_face)) {
            face.updateResolution(72, 72);
        };
        RenderedText renderText (const std::string& text, int size, int x, int y, glm::vec3 colour);
        void loadAtlas (Renderer* renderer);
        const Texture& getAtlasTexture () const;

        Font(Font&& font) noexcept : face(std::move(font.face)), atlas(std::move(font.atlas)) {
            screenX = font.screenX;
            screenY = font.screenY;
        }
    };
}
