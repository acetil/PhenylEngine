#include "graphics/ui/nodes/ui_label.h"

#include <utility>
#include "graphics/ui/ui_manager.h"

using namespace phenyl::graphics::ui;

void UILabelNode::render (UIManager& uiManager) {
    if (doTextRender) {
        renderedText = uiManager.getRenderedText(font, text, textSize, colour);
        doTextRender = false;
    }

    uiManager.renderText(renderedText, 0, 0);
    if (doDebugRender) {
        uiManager.renderRect({0, 0}, renderedText.getSize(), {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 0.0f,
                             1.0f);
    }
}

UIAnchor UILabelNode::getAnchor () {
    return UIAnchor().withFixedSize(renderedText.getSize());
}

void UILabelNode::setText (std::string newText) {
    text = std::move(newText);
    doTextRender = true;
}

void UILabelNode::setFont (std::string newFont) {
    font = std::move(newFont);
    doTextRender = true;
}

void UILabelNode::setTextSize (int newTextSize) {
    textSize = newTextSize;
    doTextRender = true;
}

void UILabelNode::setColour (glm::vec4 newColour) {
    colour = newColour;
    doTextRender = true;
}

void UILabelNode::setDebug (bool isDebug) {
    doDebugRender = isDebug;
}

void UILabelNode::onThemeUpdate (Theme* theme) {
    font = getTheme().getProperty<std::string>("font").orElse("noto-serif");
    textSize = getTheme().getProperty<int>("text_size").orElse(12);
    colour = getTheme().getProperty<glm::vec4>("text_color").orElse({0.0f, 0.0f, 0.0f, 0.0f});
    doTextRender = true;
}

const std::string& UILabelNode::getCurrentText () {
    return text;
}
