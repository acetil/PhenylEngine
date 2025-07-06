#include "graphics/font/font_manager.h"

#include "core/assets/assets.h"
#include "graphics/detail/loggers.h"
#include "graphics/font/harfbuzz_headers.h"

#include <cstring>
#include <iostream>

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"FONT_MANAGER", detail::GRAPHICS_LOGGER};

FontManager::FontManager (const Viewport& viewport, GlyphAtlas& glyphAtlas) :
    m_viewport{viewport},
    m_glyphAtlas{glyphAtlas} {
    FT_Init_FreeType(&m_library);
}

FontManager::FontManager (FontManager&& other) noexcept :
    m_viewport{other.m_viewport},
    m_library{other.m_library},
    m_glyphAtlas{other.m_glyphAtlas} {
    other.m_library = nullptr;
}

FontManager& FontManager::operator= (FontManager&& other) noexcept {
    if (m_library) {
        FT_Done_FreeType(m_library);
    }

    m_library = other.m_library;

    other.m_library = nullptr;

    return *this;
}

FontManager::~FontManager () {
    m_loadedFonts.clear();

    if (m_library) {
        FT_Done_FreeType(m_library);
    }
}

const char* FontManager::getFileType () const {
    return ".ttf";
}

std::shared_ptr<Font> FontManager::load (std::ifstream& data) {
    std::vector<char> bytes{std::istreambuf_iterator{data}, std::istreambuf_iterator<char>{}};
    auto dataSize = bytes.size();
    auto fontData = std::make_unique<std::byte[]>(bytes.size());
    std::ranges::copy(bytes, reinterpret_cast<char*>(fontData.get()));

    FT_Face face;
    auto error = FT_New_Memory_Face(m_library, reinterpret_cast<const FT_Byte*>(fontData.get()),
        static_cast<FT_Long>(dataSize), 0, &face);
    if (error) {
        PHENYL_LOGE(LOGGER, "FreeType error on FT_New_Memory_Face ({}): {}", error, FT_Error_String(error));
        return nullptr;
    }

    auto font = std::make_shared<Font>(m_glyphAtlas, std::move(fontData), face, m_nextFontId++,
        m_viewport.getContentScale() * glm::vec2{96, 96});
    m_loadedFonts.emplace_back(font); // Unloading not supported yet
    return font;
}

void FontManager::selfRegister () {
    core::Assets::AddManager(this);
}
