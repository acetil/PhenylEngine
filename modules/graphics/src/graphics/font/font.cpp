#include "graphics/font/harfbuzz_headers.h"
#include "graphics/font/font.h"
#include "graphics/font/glyph_atlas.h"
#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"FONT", detail::GRAPHICS_LOGGER};

Font::Font (GlyphAtlas& atlas, IGlyphRenderer& glyphRenderer, std::unique_ptr<std::byte[]> faceData, FT_Face face, std::size_t fontId, glm::ivec2 windowDPI) : atlas{atlas}, glyphRenderer{glyphRenderer}, faceData{std::move(faceData)}, face{face}, fontId{fontId}, windowDPI{windowDPI} {
    PHENYL_DASSERT(face);
}

Font::Font (Font&& other) noexcept : atlas{other.atlas}, glyphRenderer{other.glyphRenderer}, faceData{std::move(other.faceData)}, face{other.face}, fontId{other.fontId}, windowDPI{other.windowDPI}, currSize{other.currSize}, kernCache{std::move(other.kernCache)} {
    other.face = nullptr;
    other.fontId = 0;
}

Font& Font::operator= (Font&& other) noexcept {
    if (face) {
        FT_Done_Face(face);
    }

    faceData = std::move(other.faceData);
    face = other.face;
    fontId = other.fontId;
    windowDPI = other.windowDPI;
    currSize = other.currSize;
    kernCache = std::move(other.kernCache);

    other.face = nullptr;
    other.fontId = 0;

    return *this;
}

Font::~Font () {
    if (face) {
        FT_Done_Face(face);
    }
}

float Font::getKerning (std::uint32_t size, char32_t prev, char32_t next) {
    auto& cache = kernCache[size];

    auto it = cache.find(std::pair{prev, next});
    if (it != cache.end()) {
        return it->second;
    }

    auto prevIndex = getCharIndex(prev);
    auto nextIndex = getCharIndex(next);

    setSize(size);

    FT_Vector kerning;
    auto error = FT_Get_Kerning(face, prevIndex, nextIndex, FT_KERNING_DEFAULT, &kerning);
    if (error) {
        PHENYL_LOGE(LOGGER, "FreeType error on FT_Get_Kerning ({}): {}", error, FT_Error_String(error));
        return 0;
    }

    cache.emplace(std::pair{prev, next}, static_cast<float>(kerning.x) / 64);

    return static_cast<float>(kerning.x) / 64;
}

const Glyph& Font::getGlyph (std::uint32_t size, char32_t charCode) {
    static Glyph EMPTY_GLYPH{
        .uvStart = {0, 0},
        .uvEnd = {0, 0},
        .atlasLayer = 0,
        .advance = 0.0f,
        .bearing = {0, 0},
        .size = {0, 0}
    };

    auto& sizeMap = glyphMap[size];

    auto charIt = sizeMap.find(charCode);
    if (charIt != sizeMap.end()) {
        return charIt->second;
    }

    setSize(size);

    auto slot = face->glyph;
    auto glyphIndex = getCharIndex(charCode);

    auto error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
    if (error) {
        PHENYL_LOGE(LOGGER, "FreeType error on FT_Load_Glyph ({}): {}", error, FT_Error_String(error));
        return EMPTY_GLYPH;
    }

    auto image = Image::MakeNonOwning(std::span{reinterpret_cast<std::byte*>(slot->bitmap.buffer), slot->bitmap.width * slot->bitmap.rows}, slot->bitmap.width, slot->bitmap.rows, ImageFormat::R);
    auto placement = atlas.placeGlyph(image);

    return sizeMap[charCode] = Glyph{
            .uvStart = placement.uvStart,
            .uvEnd = placement.uvEnd,
            .atlasLayer = placement.atlasLayer,
            .advance = static_cast<float>(slot->advance.x) / 64.0f,
            .bearing = glm::ivec2{slot->bitmap_left, slot->bitmap_top},
            .size = {slot->bitmap.width, slot->bitmap.rows}
    };
}

TextBounds Font::getBounds (std::uint32_t size, std::string_view text) {
    if (text.empty()) {
        return TextBounds{
                .size = {0, 0},
                .baselineOffset = {0, 0}
        };
    }

    char prev = text[0];
    const auto& prevGlyph = getGlyph(size, text[0]);
    float minX = 0;
    float minY = -prevGlyph.bearing.y;
    float maxX = prevGlyph.bearing.x + prevGlyph.size.x;
    float maxY = minY + prevGlyph.size.y;

    float firstY = prevGlyph.bearing.y;
    float currX = prevGlyph.advance;

    for (std::size_t i = 1; i < text.size(); i++) {
        const auto& currGlyph = getGlyph(size, text[i + 1]);

        currX += getKerning(size, prev, text[i]);
        minY = std::min(minY, (float)-currGlyph.bearing.y);
        maxY = std::max(maxY, (float)-currGlyph.bearing.y + currGlyph.size.y);

        maxX = currX + currGlyph.bearing.x + currGlyph.size.x;
        currX += currGlyph.advance;

        prev = text[i];
    }

    return TextBounds{
            .size = glm::ivec2{static_cast<std::int32_t>(std::ceil(maxX - minX)), static_cast<std::int32_t>(std::ceil(maxY - minY))},
            .baselineOffset = glm::ivec2{-minX, firstY}
    };
}

void Font::renderText (std::uint32_t size, std::string_view text, glm::vec2 pos, glm::vec3 colour) {
    auto bounds = getBounds(size, text);

    std::optional<char> prev;
    float currX = pos.x + bounds.baselineOffset.x;
    for (auto c: text) {
        if (prev) {
            currX += getKerning(size, *prev, c);
        }

        const auto& glyph = getGlyph(size, c);
        glyphRenderer.renderGlyph(glyph, glm::vec2{currX + glyph.bearing.x, pos.y + bounds.baselineOffset.y - glyph.bearing.y},  colour);

        currX += glyph.advance;
        prev = c;
    }
}

void Font::setSize (std::uint32_t size) {
    if (currSize != size) {
        FT_Set_Char_Size(face, size * 64, size * 64, windowDPI.x, windowDPI.y);
        currSize = size;
    }
}

std::uint32_t Font::getCharIndex (char32_t c) {
    auto indexIt = indexCache.find(c);
    if (indexIt != indexCache.end()) {
        return indexIt->second;
    }

    return indexCache[c] = FT_Get_Char_Index(face, c);
}


