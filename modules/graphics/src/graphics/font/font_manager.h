#pragma once

#include "common/assets/asset_manager.h"
#include "graphics/font/font.h"
#include "graphics/font/glyph_atlas.h"

namespace phenyl::graphics {
    class FontManager : public common::AssetManager<Font> {
    private:
        const Viewport& viewport;
        FT_Library library = nullptr;
        GlyphAtlas& glyphAtlas;
        std::unordered_map<std::size_t, std::unique_ptr<Font>> fonts;
        std::size_t nextFontId = 1;
    public:
        explicit FontManager (const Viewport& viewport, GlyphAtlas& atlas);

        FontManager (const FontManager&) = delete;
        FontManager (FontManager&& other) noexcept;

        FontManager& operator= (const FontManager&) = delete;
        FontManager& operator= (FontManager&& other) noexcept;

        ~FontManager() override;

        const char* getFileType() const override;
        Font* load (std::ifstream& data, std::size_t id) override;
        Font* load (Font&& obj, std::size_t id) override;

        common::Asset<Font> loadFromMemory (std::span<std::byte> faceData, const std::string& virtualPath);

        void queueUnload(std::size_t id) override;
        bool isBinary() const override {
            return true;
        }

        void selfRegister ();
    };

}