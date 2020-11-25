#include <utility>
#include <vector>

#include "glyph_image.h"
#ifndef GLYPH_ATLAS_H
#define GLYPH_ATLAS_H
namespace graphics {
    class GlyphAtlas {
    private:
        unsigned char* data = nullptr;
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        int width = 0;
        int height = 0;
    public:
        GlyphAtlas () = default;
        GlyphAtlas (const std::vector<GlyphImage>& glyphs, int targetRes);
        GlyphAtlas (GlyphAtlas& atlas) = delete;
        GlyphAtlas (GlyphAtlas&& atlas) : data(std::exchange(atlas.data, nullptr)), offsetX(atlas.offsetX),
                                          offsetY(atlas.offsetY) {}

        ~GlyphAtlas ();
    };
}
#endif //GLYPH_ATLAS_H
