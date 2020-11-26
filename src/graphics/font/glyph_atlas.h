#include <utility>
#include <vector>

#include "glyph_image.h"
#include "graphics/renderers/renderer.h"
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
        GraphicsTexture texture;
    public:
        GlyphAtlas () = default;
        GlyphAtlas (const std::vector<GlyphImage>& glyphs, int targetRes);
        GlyphAtlas (GlyphAtlas& atlas) = delete;
        GlyphAtlas (GlyphAtlas&& atlas)  noexcept : data(std::exchange(atlas.data, nullptr)), offsetX(atlas.offsetX),
                offsetY(atlas.offsetY) {}
        GlyphAtlas& operator= (GlyphAtlas&& atlas)  noexcept {
            offsetX = atlas.offsetX;
            offsetY = atlas.offsetY;
            width = atlas.width;
            height = atlas.height;
            texture = atlas.texture;
            data = std::exchange(atlas.data, nullptr);
            return *this;
        }
        void loadAtlas (Renderer* renderer);
        GraphicsTexture& getTex () {
            return texture;
        }
        ~GlyphAtlas ();
    };
}
#endif //GLYPH_ATLAS_H
