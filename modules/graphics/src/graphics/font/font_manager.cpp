#include "logging/logging.h"
#include "util/exceptions.h"
#include "graphics/font/harfbuzz_headers.h"
#include "graphics/font/font_manager.h"

using namespace phenyl::graphics;

FontManager::FontManager () {
    auto error = FT_Init_FreeType(&freetypeLib);
    if (error) {
        throw util::InitException("Freetype lib init exception");
    }
}

FontManager::~FontManager () {
    fontFaces.clear();
    if (freetypeLib) {
        FT_Done_FreeType(freetypeLib);
    }
}

void FontManager::addFace (const std::string& face, const std::string& path, int faceNum) {
    try {
        fontFaces.insert(std::make_pair(face, FontFace(freetypeLib, path, faceNum)));
    } catch (util::InitException& e) {
        logging::log(LEVEL_ERROR, "Error creating font \"{}\": ", face);
        logging::log(LEVEL_ERROR, e.getMsg());
    }
}

FontManager::FontManager (FontManager&& manager) noexcept : freetypeLib(std::exchange(manager.freetypeLib, nullptr)),
                                                            fontFaces(std::move(manager.fontFaces)) {

}

FontFace& FontManager::getFace (const std::string& face) {
    return fontFaces[face];
}
