#pragma once

#include <string>
#include <unordered_map>
#include "harfbuzz_typedefs.h"
#include "font_face.h"

namespace graphics {
    class FontManager {
    private:
        FT_Library freetypeLib = nullptr;
        bool ownsMemory = true;
        std::unordered_map<std::string, FontFace> fontFaces;
    public:
        FontManager ();
        FontManager (FontManager& manager) = delete;
        FontManager (FontManager&& manager) noexcept ;
        ~FontManager ();

        void addFace (const std::string& face, const std::string& path, int faceNum = 0);
        FontFace& getFace (const std::string& face);
    };
}
