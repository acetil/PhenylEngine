#include "common/assets/assets.h"
#include "graphics/ui/nodes/ui_label.h"

#include <utility>
#include "graphics/ui/ui_manager.h"

using namespace phenyl::graphics::ui;

UILabelNode::UILabelNode (const std::string& themeClass) : UIComponentNode(themeClass) {
    //font = common::Assets::Load<Font>("/usr/share/fonts/noto/NotoSerif-Regular");
}


void UILabelNode::render (UIManager& uiManager) {
    if (!font) {
        font = common::Assets::Load<Font>("/usr/share/fonts/noto/NotoSerif-Regular");
    }
    auto renderedText = font->renderText(text, textSize, 0, 0, colour);
    /*if (doTextRender) {
        renderedText = uiManager.getRenderedText(font, text, textSize, colour);
        doTextRender = false;
    }*/

    labelSize = renderedText.getSize();

    uiManager.renderText(std::move(renderedText), 0, 0);
    if (doDebugRender) {
        uiManager.renderRect({0, 0}, labelSize, {0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 0.0f,
                             1.0f);
    }
}

UIAnchor UILabelNode::getAnchor () {
    return UIAnchor().withFixedSize(labelSize);
}

void UILabelNode::setText (std::string newText) {
    text = std::move(newText);
    doTextRender = true;
}

void UILabelNode::setFont (const std::string& newFont) {
    font = common::Assets::Load<Font>(newFont);
    PHENYL_ASSERT(font);

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

void UILabelNode::onThemeUpdate () {
    font = common::Assets::Load<Font>(getTheme().getProperty<std::string>("font").orElse("/usr/share/fonts/noto/NotoSerif-Regular"));
    textSize = getTheme().getProperty<int>("text_size").orElse(12);
    colour = getTheme().getProperty<glm::vec4>("text_color").orElse({0.0f, 0.0f, 0.0f, 0.0f});
    doTextRender = true;
}

const std::string& UILabelNode::getCurrentText () {
    return text;
}