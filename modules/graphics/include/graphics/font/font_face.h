#pragma once

#include <string>
#include <vector>
#include <utility>
#include "harfbuzz_typedefs.h"
#include "glyph_image.h"

namespace phenyl::graphics {
    struct CharOffsets {
        int advance;
        int offsetX;
        int offsetY;
        int width;
        int height;
        CharOffsets (int _offsetX, int _offsetY, int _width, int _height, int _advance) : offsetX(_offsetX),
                offsetY(_offsetY), width(_width), height(_height), advance(_advance) {}
    };
    //constexpr std::pair<int, int> AsciiGlyphRange = {33, 127};
    constexpr std::pair<int, int> AsciiGlyphRange = {32, 255};
    class FontFace {
    private:
        hb_face_t* fontFace = nullptr;
        FT_Face ftFace = nullptr;
        int size = 11;
        int xRes = 800;
        int yRes = 600;
        std::vector<std::pair<int, int>> glyphRanges;
    public:
        FontFace () = default;
        FontFace (const FT_Library& library, const std::string& path, int faceNum);
        FontFace(FontFace& face) = delete;
        FontFace(FontFace&& face) noexcept;
        ~FontFace();
        int getNumGlyphs ();
        std::vector<int> getFixedFontSizes ();
        void setFontSize (int _size);
        void setGlyphs (std::vector<std::pair<int, int>> _glyphRanges);
        void updateResolution (int _xRes, int _yRes);
        std::vector<GlyphImage> getGlyphs ();
        CharOffsets renderText (int c, int prev);
    };
}
