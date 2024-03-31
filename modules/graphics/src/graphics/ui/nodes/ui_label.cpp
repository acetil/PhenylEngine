#include "common/assets/assets.h"
#include "graphics/ui/nodes/ui_label.h"
#include "graphics/font/font_manager.h"

#include <utility>
#include "graphics/ui/ui_manager.h"

using namespace phenyl::graphics::ui;

static phenyl::Logger LOGGER{"UI_LABEL", phenyl::graphics::detail::GRAPHICS_LOGGER};

UILabelNode::UILabelNode (const std::string& themeClass) : UIComponentNode(themeClass) {
}


void UILabelNode::render (UIManager& uiManager) {
    if (!font) {
        PHENYL_LOGW(LOGGER, "No font configured for label!");
        return;
    }
    //auto renderedText = font->renderText(text, textSize, 0, 0, colour);
    /*if (doTextRender) {
        renderedText = uiManager.getRenderedText(font, text, textSize, colour);
        doTextRender = false;
    }*/

    //uiManager.renderText(std::move(renderedText), 0, 0);
    uiManager.renderText(font, textSize, text, {0, 0}, colour);
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

    if (font) {
        labelSize = font->getBounds(textSize, text).size;
    }
}

void UILabelNode::setFont (const std::string& newFont) {
    font = common::Assets::Load<Font>(newFont);
    PHENYL_ASSERT(font);

    labelSize = font->getBounds(textSize, text).size;
}

void UILabelNode::setTextSize (int newTextSize) {
    textSize = newTextSize;

    if (font) {
        labelSize = font->getBounds(textSize, text).size;
    }
}

void UILabelNode::setColour (glm::vec4 newColour) {
    colour = newColour;
}

void UILabelNode::setDebug (bool isDebug) {
    doDebugRender = isDebug;
}

void UILabelNode::onThemeUpdate () {
    font = common::Assets::Load<Font>(getTheme().getProperty<std::string>("font").orElse("/usr/share/fonts/noto/NotoSerif-Regular"));
    textSize = getTheme().getProperty<int>("text_size").orElse(12);
    colour = getTheme().getProperty<glm::vec4>("text_color").orElse({0.0f, 0.0f, 0.0f, 0.0f});

    labelSize = font->getBounds(textSize, text).size;
}

const std::string& UILabelNode::getCurrentText () {
    return text;
}