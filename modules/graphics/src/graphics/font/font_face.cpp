#include <stdlib.h>

#include "util/exceptions.h"
#include "graphics/detail/loggers.h"
#include "graphics/font/harfbuzz_headers.h"
#include "graphics/font/font_face.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"FONT_FACE", detail::GRAPHICS_LOGGER};

FontFace::FontFace (const FT_Library& library, const std::string& path, int faceNum) {
    auto error = FT_New_Face(library, path.c_str(), faceNum, &ftFace);
    if (error) {
        throw util::InitException("Error in init of font face at path \"" + path + "\"");
    }
    error = FT_Set_Char_Size(ftFace, 0, size * 64, 0, 0); // TODO
    if (error) {
        PHENYL_LOGE(LOGGER, "Error encountered when setting char size: {}", error);
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
                                                ftFace(std::exchange(face.ftFace,nullptr)), glyphRanges(std::move(face.glyphRanges)), size(face.size) {

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
    FT_Set_Char_Size(ftFace, 0, size * 64, xRes, yRes);
}

void FontFace::setGlyphs (std::vector<std::pair<int, int>> _glyphRanges) {
    //logging::log(LEVEL_DEBUG, "Num ranges: {}", _glyphRanges.size());
    glyphRanges.reserve(_glyphRanges.size());
    std::copy(_glyphRanges.begin(), _glyphRanges.end(), std::back_inserter(glyphRanges));
}

void FontFace::setFontSize (int _size) {
    size = _size;
    int error = FT_Set_Char_Size(ftFace, 0, size * 64, 0, 0);
    if (error) {
        PHENYL_LOGE(LOGGER, "Error encountered while setting char size: {}", error);
    }
}

std::vector<GlyphImage> FontFace::getGlyphs () {
    std::vector<GlyphImage> glyphs;
    /*for (auto p : glyphRanges) {
        for (int i = p.first; i < p.second; i++) {

        }
    }*/
    int error;
    error = FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
    if (error) {
        PHENYL_LOGE(LOGGER, "Charmap select encountered error code {}!", error);
    }

    error = FT_Load_Glyph(ftFace, 0, FT_LOAD_DEFAULT);
    if (error) {
        PHENYL_LOGE(LOGGER, "Load char encountered error code {}!", error);
    }
    error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
    if (error) {
        PHENYL_LOGE(LOGGER, "Render glyph encountered error code {}!", error);
    }
    auto buf = new unsigned char[ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows];
    memcpy(buf, ftFace->glyph->bitmap.buffer, ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows);
    glyphs.emplace_back(buf, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows,
                        0, size, 300, 300);

    for (auto p : glyphRanges) {
        for (int i = p.first; i < p.second; i++) {
            //auto index = FT_Get_Char_Index(ftFace, 'A');
            //logging::log(LEVEL_DEBUG, "Char index: {}", index);
            error = FT_Load_Char(ftFace, i, FT_LOAD_DEFAULT);
            if (error) {
                PHENYL_LOGE(LOGGER, "Load char encountered error code {}!", error);
            }
            error = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
            if (error) {
                PHENYL_LOGE(LOGGER, "Render glyph encountered error code {}!", error);
            }
            auto buf2 = new unsigned char[ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows];
            memcpy(buf2, ftFace->glyph->bitmap.buffer, ftFace->glyph->bitmap.width * ftFace->glyph->bitmap.rows);
            glyphs.emplace_back(buf2, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows,
                                           i, size, 300, 300);
        }
    }
    //logging::log(LEVEL_DEBUG, "Gotten all glyphs!");
    //logging::log(LEVEL_DEBUG, "Rendered glyph {} with pixel format grey? {} ", 'W',
                 //ftFace->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY);
    return glyphs;
}

CharOffsets FontFace::renderText (int c, int prev) {
    if (int error = FT_Load_Char(ftFace, c, FT_LOAD_BITMAP_METRICS_ONLY)) {
        PHENYL_LOGE(LOGGER, "Load char encountered error code {}!", error);
        return CharOffsets(0, 0, 0, 0, 0);
    }
    FT_Vector vec;
    vec.x = 0;
    vec.y = 0;
    if (prev != 0) {
        FT_Get_Kerning(ftFace, prev, c, FT_KERNING_DEFAULT, &vec);
    }
    FT_GlyphSlot slot = ftFace->glyph;
    return CharOffsets(slot->metrics.horiBearingX / 64 + vec.x / 64, -slot->metrics.horiBearingY / 64, slot->metrics.width / 64, slot->metrics.height / 64, slot->metrics.horiAdvance / 64);
}
