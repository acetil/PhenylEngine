#include <iostream>

#include "core/assets/assets.h"
#include "graphics/font/harfbuzz_headers.h"
#include "graphics/font/font_manager.h"

#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"FONT_MANAGER", detail::GRAPHICS_LOGGER};

FontManager::FontManager (const Viewport& viewport, GlyphAtlas& glyphAtlas) : m_viewport{viewport},  m_glyphAtlas{glyphAtlas} {
    FT_Init_FreeType(&m_library);
}

FontManager::FontManager (FontManager&& other) noexcept : m_viewport{other.m_viewport}, m_library{other.m_library}, m_glyphAtlas{other.m_glyphAtlas}, m_fonts{std::move(other.m_fonts)} {
    other.m_library = nullptr;
}

FontManager& FontManager::operator= (FontManager&& other) noexcept {
    if (m_library) {
        FT_Done_FreeType(m_library);
    }

    m_library = other.m_library;
    m_fonts = std::move(other.m_fonts);

    other.m_library = nullptr;

    return *this;
}

FontManager::~FontManager () {
    m_fonts.clear();

    if (m_library) {
        FT_Done_FreeType(m_library);
    }
}

const char* FontManager::getFileType () const {
    return ".ttf";
}

Font* FontManager::load (std::ifstream& data, std::size_t id) {
    std::vector<char> bytes{std::istreambuf_iterator<char>{data}, std::istreambuf_iterator<char>{}};
    auto dataSize = bytes.size();
    auto fontData = std::make_unique<std::byte[]>(bytes.size());
    std::memcpy(fontData.get(), bytes.data(), bytes.size());

    FT_Face face;
    auto error = FT_New_Memory_Face(m_library, reinterpret_cast<const FT_Byte*>(fontData.get()),
                                    static_cast<FT_Long>(dataSize), 0, &face);
    if (error) {
        PHENYL_LOGE(LOGGER, "FreeType error on FT_New_Memory_Face ({}): {}", error, FT_Error_String(error));
        return nullptr;
    }

    auto font = std::make_unique<Font>(m_glyphAtlas, std::move(fontData), face, m_nextFontId++, m_viewport.getContentScale() * glm::vec2{96, 96});
    auto* ptr = font.get();
    m_fonts.emplace(id, std::move(font));

    return ptr;
}

Font* FontManager::load (Font&& obj, std::size_t id) {
    auto font = std::make_unique<Font>(std::move(obj));
    auto* ptr = font.get();
    m_fonts.emplace(id, std::move(font));

    return ptr;
}

void FontManager::queueUnload (std::size_t id) {
    // Do nothing
}

void FontManager::selfRegister () {
    core::Assets::AddManager(this);
}
