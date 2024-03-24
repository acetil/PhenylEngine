#include <iostream>

#include "common/assets/assets.h"
#include "logging/logging.h"
#include "util/exceptions.h"
#include "graphics/detail/loggers.h"
#include "graphics/font/harfbuzz_headers.h"
#include "graphics/font/font_manager.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"FONT_MANAGER", detail::GRAPHICS_LOGGER};

FontManager::FontManager () {
    auto error = FT_Init_FreeType(&freetypeLib);
    if (error) {
        throw util::InitException("Freetype lib init exception");
    }
}

FontManager::~FontManager () {
    common::Assets::RemoveManager(this);

    fontFaces.clear();
    if (freetypeLib) {
        FT_Done_FreeType(freetypeLib);
    }
}

void FontManager::addFace (const std::string& face, const std::string& path, int faceNum) {
    try {
        fontFaces.insert(std::make_pair(face, FontFace(freetypeLib, path, faceNum)));
    } catch (util::InitException& e) {
        PHENYL_LOGW(LOGGER,  "Error creating font \"{}\": {}", face, e.getMsg());
    }
}

FontManager::FontManager (FontManager&& manager) noexcept : freetypeLib(std::exchange(manager.freetypeLib, nullptr)),
                                                            fontFaces(std::move(manager.fontFaces)) {

}

FontFace& FontManager::getFace (const std::string& face) {
    return fontFaces[face];
}

const char* FontManager::getFileType () const {
    return ".ttf";
}

Font* FontManager::load (std::istream& file, std::size_t id) {
    std::vector<unsigned char> data{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};

    FontFace face{freetypeLib, std::move(data)};
    face.setFontSize(72);
    face.setGlyphs({AsciiGlyphRange});
    auto font = std::make_unique<Font>(std::move(face), 128); // TODO

    auto* ptr = font.get();
    fonts[id] = std::move(font);

    return ptr;
}

Font* FontManager::load (Font&& obj, std::size_t id) {
    auto font = std::make_unique<Font>(std::move(obj));

    auto* ptr = font.get();
    fonts[id] = std::move(font);

    return ptr;
}

void FontManager::queueUnload (std::size_t id) {
    fonts.erase(id);
}

void FontManager::selfRegister () {
    common::Assets::AddManager(this);
}
