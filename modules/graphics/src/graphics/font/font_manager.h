#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/font/font.h"
#include "graphics/font/glyph_atlas.h"

namespace phenyl::graphics {
class FontManager : public core::AssetManager<Font> {
public:
    explicit FontManager (const Viewport& viewport, GlyphAtlas& atlas);

    FontManager (const FontManager&) = delete;
    FontManager (FontManager&& other) noexcept;

    FontManager& operator= (const FontManager&) = delete;
    FontManager& operator= (FontManager&& other) noexcept;

    ~FontManager () override;

    const char* getFileType () const override;
    std::shared_ptr<Font> load (std::ifstream& data) override;

    bool isBinary () const override {
        return true;
    }

    void selfRegister ();

private:
    const Viewport& m_viewport;
    FT_Library m_library = nullptr;
    GlyphAtlas& m_glyphAtlas;
    std::vector<std::shared_ptr<Font>> m_loadedFonts;
    std::size_t m_nextFontId = 1;
};

} // namespace phenyl::graphics
