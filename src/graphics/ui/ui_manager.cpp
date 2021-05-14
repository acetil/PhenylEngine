#include "ui_manager.h"
#include "graphics/renderers/renderer.h"
#include "graphics/graphics.h"
#include "graphics/renderlayer/ui_layer.h"

#include "logging/logging.h"

using namespace graphics;


graphics::UIManager::UIManager (Renderer* renderer, FontManager& _fontManager) : fontManager(std::move(_fontManager)) {

    auto f = Font(fontManager.getFace("noto-serif"), 128);

    fonts.insert(std::make_pair("noto-serif", std::move(f)));

    //fonts["noto-serif"] = std::move(Font(fontManager.getFace("noto-serif"), 128)));

    fonts.at("noto-serif").loadAtlas(renderer);
    uiLayer = nullptr;
}

void UIManager::renderText (const std::string& font, const std::string& text, int size, int x, int y) {
    if (!fonts.contains(font)) {
        logging::log(LEVEL_ERROR, "Font {} does not exist!", font);
    } else {
        uiLayer->bufferStr(fonts.at(font), text, size, x, y);
    }
}

void UIManager::addRenderLayer (Graphics* graphics, Renderer* renderer) {
    uiLayer = new UIRenderLayer(fonts.at("noto-serif").getAtlasTexture(), renderer);
    graphics->getRenderLayer()->addRenderLayer(uiLayer);
}

