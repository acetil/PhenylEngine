#pragma once

#include <utility>
#include <vector>
#include <unordered_map>

#include "util/span.h"
#include "glyph_image.h"
#include "graphics/renderers/renderer.h"

namespace graphics {
    class GlyphAtlas {
    private:
        unsigned char* data = nullptr;
        float* uvs = nullptr;
        std::unordered_map<int, util::span<float>> charUvs;
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        int width = 0;
        int height = 0;
        GraphicsTexture texture;
    public:
        GlyphAtlas () = default;
        GlyphAtlas (const std::vector<GlyphImage>& glyphs, int targetRes);
        GlyphAtlas (GlyphAtlas& atlas) = delete;
        GlyphAtlas (GlyphAtlas&& atlas) noexcept : data(std::exchange(atlas.data, nullptr)),
                                                   offsetX(atlas.offsetX),
                                                   offsetY(atlas.offsetY),
                                                   uvs(std::exchange(atlas.uvs, nullptr)),
                                                   charUvs(std::move(atlas.charUvs)),
                                                   width(atlas.width),
                                                   height(atlas.height) {}

        GlyphAtlas& operator= (GlyphAtlas&& atlas)  noexcept {
            offsetX = atlas.offsetX;
            offsetY = atlas.offsetY;
            width = atlas.width;
            height = atlas.height;
            texture = atlas.texture;
            data = std::exchange(atlas.data, nullptr);
            uvs = std::exchange(atlas.uvs, nullptr);
            charUvs = std::move(atlas.charUvs);
            return *this;
        }
        void loadAtlas (Renderer* renderer);
        GraphicsTexture& getTex () {
            return texture;
        }
        void bufferChar (Buffer& uvBuffer, int c);
        ~GlyphAtlas ();
    };
}
