#include <string>
#include <vector>
#include <utility>
#include "harfbuzz_typedefs.h"
#include "glyph_image.h"
#ifndef FONT_FACE_H
#define FONT_FACE_H
namespace graphics {
    //constexpr std::pair<int, int> AsciiGlyphRange = {33, 127};
    constexpr std::pair<int, int> AsciiGlyphRange = {33, 255};
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
    };
}
#endif
