#include <iostream>

#include "core/assets/assets.h"
#include "graphics/font/harfbuzz_headers.h"
#include "graphics/font/font_manager.h"

#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"FONT_MANAGER", detail::GRAPHICS_LOGGER};

FontManager::FontManager (const Viewport& viewport, GlyphAtlas& glyphAtlas) : viewport{viewport},  glyphAtlas{glyphAtlas} {
    FT_Init_FreeType(&library);
}

FontManager::FontManager (FontManager&& other) noexcept : viewport{other.viewport}, library{other.library}, glyphAtlas{other.glyphAtlas}, fonts{std::move(other.fonts)} {
    other.library = nullptr;
}

FontManager& FontManager::operator= (FontManager&& other) noexcept {
    if (library) {
        FT_Done_FreeType(library);
    }

    library = other.library;
    fonts = std::move(other.fonts);

    other.library = nullptr;

    return *this;
}

FontManager::~FontManager () {
    fonts.clear();

    if (library) {
        FT_Done_FreeType(library);
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
    auto error = FT_New_Memory_Face(library, reinterpret_cast<const FT_Byte*>(fontData.get()),
                                    static_cast<FT_Long>(dataSize), 0, &face);
    if (error) {
        PHENYL_LOGE(LOGGER, "FreeType error on FT_New_Memory_Face ({}): {}", error, FT_Error_String(error));
        return nullptr;
    }

    auto font = std::make_unique<Font>(glyphAtlas, std::move(fontData), face, nextFontId++, viewport.getContentScale() * glm::vec2{96, 96});
    auto* ptr = font.get();
    fonts.emplace(id, std::move(font));

    return ptr;
}

Font* FontManager::load (Font&& obj, std::size_t id) {
    auto font = std::make_unique<Font>(std::move(obj));
    auto* ptr = font.get();
    fonts.emplace(id, std::move(font));

    return ptr;
}

void FontManager::queueUnload (std::size_t id) {
    // Do nothing
}

void FontManager::selfRegister () {
    core::Assets::AddManager(this);
}
