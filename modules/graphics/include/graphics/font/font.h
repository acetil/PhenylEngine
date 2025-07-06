#pragma once

#include "core/assets/asset.h"
#include "glyph.h"
#include "graphics/image.h"
#include "graphics/maths_headers.h"
#include "harfbuzz_typedefs.h"
#include "util/hash.h"

#include <cstdint>
#include <unordered_map>

namespace phenyl::graphics {
class GlyphAtlas;

struct TextBounds {
    glm::ivec2 size;
    glm::ivec2 baselineOffset;
};

class Font : public core::AssetType2<Font> {
public:
    Font (GlyphAtlas& atlas, std::unique_ptr<std::byte[]> faceData, FT_Face face, std::size_t fontId,
        glm::ivec2 windowDPI);
    Font (const Font&) = delete;
    Font (Font&& other) noexcept;

    Font& operator= (const Font&) = delete;
    Font& operator= (Font&& other) noexcept;

    ~Font ();

    const Glyph& getGlyph (std::uint32_t size, char32_t charCode);
    float getKerning (std::uint32_t size, char32_t prev, char32_t next);
    TextBounds getBounds (std::uint32_t size, std::string_view text);
    void renderText (IGlyphRenderer& glyphRenderer, std::uint32_t size, std::string_view text, glm::vec2 pos,
        glm::vec3 colour = glm::vec3{1.0f, 1.0f, 1.0f});

    std::size_t id () const noexcept {
        return m_id;
    }

private:
    GlyphAtlas& m_atlas;
    std::unique_ptr<std::byte[]> m_faceData;
    FT_Face m_face;
    std::size_t m_id;
    glm::ivec2 m_windowDpi;
    std::uint32_t m_size = 0;

    std::unordered_map<char32_t, std::uint32_t> m_indexCache;
    std::unordered_map<std::uint32_t, util::HashMap<std::pair<char32_t, char32_t>, float>> m_kernCache;
    std::unordered_map<std::uint32_t, std::unordered_map<char32_t, Glyph>> m_glyphMap;

    void setSize (std::uint32_t size);
    std::uint32_t getCharIndex (char32_t c);
};
} // namespace phenyl::graphics
