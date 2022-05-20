#include "graphics/ui/components/ui_label.h"

#include <utility>
#include "graphics/ui/ui_manager.h"

using namespace graphics::ui;

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
