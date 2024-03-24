#pragma once

#include <string>
#include <unordered_map>
#include "harfbuzz_typedefs.h"
#include "font_face.h"
#include "font.h"
#include "common/assets/asset_manager.h"

namespace phenyl::graphics {
    class FontManager : public common::AssetManager<Font> {
    private:
        //FT_Library freetypeLib = nullptr;
        bool ownsMemory = true;
        std::unordered_map<std::string, FontFace> fontFaces;
        std::unordered_map<std::size_t, std::unique_ptr<Font>> fonts;
    public:
        FontManager ();
        FontManager (FontManager& manager) = delete;
        FontManager (FontManager&& manager) noexcept ;
        ~FontManager ();

        void addFace (const std::string& face, const std::string& path, int faceNum = 0);
        FontFace& getFace (const std::string& face);

        const char* getFileType () const override;
        Font* load (std::istream& file, std::size_t id) override;
        Font* load (phenyl::graphics::Font&& obj, std::size_t id) override;
        void queueUnload (std::size_t id) override;
        bool isBinary() const override {
            return true;
        }

        void selfRegister ();

        FT_Library freetypeLib = nullptr;
    };
}
