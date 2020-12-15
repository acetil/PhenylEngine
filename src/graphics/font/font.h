#include <string>

#include "font_face.h"
#include "glyph_atlas.h"
#ifndef FONT_H
#define FONT_H
namespace graphics {
    class Font {
    private:
        FontFace face;
        GlyphAtlas atlas;
        int screenX = 800;
        int screenY = 600;
    public:
        Font (FontFace& _face, int targetRes) : atlas(face.getGlyphs(), targetRes), face(std::move(_face)) {
            face.updateResolution(72, 72);
        };
        void renderText (const std::string& text, int size, int x, int y, Buffer* buffer);
        void bindAtlasTexture ();
        void loadAtlas (Renderer* renderer);
    };
}
#endif
