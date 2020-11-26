#include <stdlib.h>

#include "util/exceptions.h"
#include "harfbuzz_headers.h"
#include "font_face.h"
#include "logging/logging.h"

using namespace graphics;

FontFace::FontFace (const FT_Library& library, const std::string& path, int faceNum) {
    auto error = FT_New_Face(library, path.c_str(), faceNum, &ftFace);
    if (error) {
        throw util::InitException("Error in init of font face at path \"" + path + "\"");
    }
    error = FT_Set_Char_Size(ftFace, 0, size * 64, 300, 300); // TODO
    if (error) {
        logging::log(LEVEL_ERROR, "Error encountered when setting char size: {}", error);
    }
    fontFace = hb_ft_face_create(ftFace, nullptr);
}

FontFace::~FontFace () {
    if (fontFace) {
        hb_face_destroy(fontFace);
    }
    if (ftFace) {
        FT_Done_Face(ftFace);
    }
}

FontFace::FontFace (FontFace&& face) noexcept : fontFace(std::exchange(face.fontFace, nullptr)),
                                                ftFace(std::exchange(face.ftFace,nullptr)) {

}

int FontFace::getNumGlyphs () {
    if (ftFace == nullptr) {
        return 0;
    }
    return ftFace->num_glyphs;
}

std::vector<int> FontFace::getFixedFontSizes () {
    std::vector<int> sizes;
    sizes.reserve(ftFace->num_fixed_sizes);
    for (int i = 0; i < ftFace->num_fixed_sizes; i++) {
        sizes.emplace_back(ftFace->available_sizes[i].size);
    }
    return sizes;
}
void FontFace::updateResolution (int _xRes, int _yRes) {
    this->xRes = _xRes;
    this->yRes = _yRes;
    FT_Set_Char_Size(ftFace, 0, size * 64, 300, 300);
}

void FontFace::setGlyphs (std::vector<std::pair<int, int>> _glyphRanges) {
    std::copy(glyphRanges.begin(), _glyphRanges.begin(), _glyphRanges.end());
}

void FontFace::setFontSize (int _size) {
    size = _size;
    int error = FT_Set_Char_Size(ftFace, 0, size * 64, 300, 300);
    if (error) {
        logging::log(LEVEL_ERROR, "Error encountered while setting char size: {}", error);
    }
}

std::vector<GlyphImage> FontFace::getGlyphs () {
    std::vector<GlyphImage> glyphs;
    /*for (auto p : glyphRanges) {
        for (int i = p.first; i < p.second; i++) {

        }
    }*/
    // Test glyph a
    int error;
    error = FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
    if (error) {
        logging::log(LEVEL_ERROR, "Charmap select encountered error code {}!", error);
    }
    //auto index = FT_Get_Char_Index(ftFace, 'A');
    //logging::log(LEVEL_DEBUG, "Char index: {}", index);
    error = FT_Load_Char(ftFace, 'a', FT_LOAD_DEFAULT);
    if (error) {
        logging::log(LEVEL_ERROR, "Load char encountered error code {}!", error);
    }
    error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        logging::log(LEVEL_ERROR, "Render glyph encountered error code {}!", error);
    }
    auto buf = new unsigned char[ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows];
    memcpy(buf, ftFace->glyph->bitmap.buffer, ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows);
    glyphs.emplace_back(GlyphImage(buf, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows,
                                   'a', size, 300, 300));
    //logging::log(LEVEL_DEBUG, "Rendered glyph {} with pixel format grey? {} ", 'W',
                 //ftFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
    return glyphs;
}
