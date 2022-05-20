#include "graphics/ui/ui_manager.h"
#include "graphics/renderers/renderer.h"
#include "graphics/graphics.h"
#include "graphics/renderlayer/ui_layer.h"
#include "graphics/font/rendered_text.h"
#include "common/events/map_load.h"
#include "graphics/ui/components/ui_root.h"

#include "logging/logging.h"

using namespace graphics;


graphics::UIManager::UIManager (Renderer* renderer, FontManager& _fontManager) : fontManager(std::move(_fontManager)) {

    auto f = Font(fontManager.getFace("noto-serif"), 128);

    fonts.insert(std::make_pair("noto-serif", std::move(f)));

    //fonts["noto-serif"] = std::move(Font(fontManager.getFace("noto-serif"), 128)));

    fonts.at("noto-serif").loadAtlas(renderer);
    uiLayer = nullptr;
    offsetStack.emplace_back(0,0);

    uiRoot = std::make_shared<ui::UIRootNode>();
}

void UIManager::renderText (const std::string& font, const std::string& text, int size, int x, int y) {
    renderText(font, text, size, x, y, {1.0f, 1.0f, 1.0f});
}

void UIManager::addRenderLayer (const detail::Graphics::SharedPtr& graphics, Renderer* renderer) {
    uiLayer = std::make_shared<UIRenderLayer>(fonts.at("noto-serif").getAtlasTexture(), renderer);
    graphics->getRenderLayer()->addRenderLayer(uiLayer);
}

/*void UIManager::renderText (const std::string& font, const std::string& text, int size, int x, int y,
                            glm::vec3 colour) {
    if (!fonts.contains(font)) {
        logging::log(LEVEL_ERROR, "Font {} does not exist!", font);
    } else {
        uiLayer->bufferStr(fonts.at(font), text, size, x, y, colour);
    }
}*/

void UIManager::renderText (const std::string& font, const std::string& text, int size, int x, int y,
                            glm::vec3 colour) {
    if (!fonts.contains(font)) {
        logging::log(LEVEL_ERROR, "Font {} does not exist!", font);
    } else {
        auto offVec = offsetStack.back() + glm::vec2{x, y};
        textBuf.emplace_back(offVec, fonts.at(font).renderText(text, size, 0, 0, colour));
    }
}

void UIManager::renderUI () {
    uiLayer->setScreenSize(screenSize);

    uiRoot->render(*this);

    for (auto& textPair : textBuf) {
        auto& text = textPair.second;
        text.setOffset(textPair.first, screenSize);
        uiLayer->bufferText(text);
    }

    for (auto& textPair : textBuf2) {
        auto& text = textPair.second;
        text.setOffset(textPair.first, screenSize);
        uiLayer->bufferText(text);
    }
    textBuf.clear();
    textBuf2.clear();
}

void UIManager::renderRect (glm::vec2 topLeftPos, glm::vec2 size, glm::vec4 bgColour, glm::vec4 borderColour, float cornerRadius, float borderSize) {
    uiLayer->bufferRect({topLeftPos + offsetStack.back(), size, borderColour, bgColour, {size, cornerRadius, borderSize}});
}

void UIManager::setMousePos (glm::vec2 _mousePos) {
    mousePos = _mousePos;
    uiRoot->setMousePos(mousePos);
}

bool UIManager::setMouseDown (bool _mouseDown) {
    if (mouseDown != _mouseDown) {
        mouseDown = _mouseDown;
        if (mouseDown) {
            return uiRoot->onMousePress();
        } else {
            uiRoot->onMouseRelease();
        }
    }
    return false;
}

void UIManager::pushOffset (glm::vec2 relOffset) {
    offsetStack.push_back(offsetStack[offsetStack.size() - 1] + relOffset);
}

void UIManager::popOffset () {
    if (offsetStack.size() == 1) {
        logging::log(LEVEL_ERROR, "Attempted to pop too many elements off offset stack!");
    } else {
        offsetStack.pop_back();
    }
}

void UIManager::addUINode (const std::shared_ptr<ui::UIComponentNode>& uiNode, glm::vec2 pos) {
    uiRoot->addChildNode(uiNode, pos);
}

RenderedText UIManager::getRenderedText (const std::string& font, const std::string& text, int size, glm::vec3 colour) {
    if (!fonts.contains(font)) {
        logging::log(LEVEL_ERROR, "Font {} does not exist!", font);
        return RenderedText{0};
    } else {
        return fonts.at(font).renderText(text, size, 0, 0, colour);
    }
}

void UIManager::renderText (RenderedText& text, int x, int y) {
    auto offVec = offsetStack.back() + glm::vec2{x, y};
    //text.setOffset(offVec, screenSize);

    //uiLayer->bufferText(text);
    textBuf2.emplace_back(offVec, text);
}

