#include "graphics/ui/nodes/ui_button.h"
#include "graphics/ui/ui_manager.h"
#include "logging/logging.h"

using namespace phenyl::graphics::ui;

static phenyl::Logger LOGGER{"UI_BUTTON", phenyl::graphics::detail::GRAPHICS_LOGGER};

void UIButtonNode::render (UIManager& uiManager) {
    glm::vec4 colour;
    if (isPressed) {
        colour = pressColour;
    } else if (isHovered) {
        colour = hoverColour;
    } else {
        colour = bgColour;
    }

    uiManager.renderRect({0, 0}, size, colour, colour);
}

UIAnchor UIButtonNode::getAnchor () {
    return UIAnchor().withFixedSize(minSize);
}

bool UIButtonNode::onMousePress () {
    if (isHovered && !isPressed) {
        PHENYL_TRACE(LOGGER, "Button pressed!");
        isPressed = true;
    }

    return isPressed;
}

void UIButtonNode::onMouseRelease () {
    if (isPressed) {
        PHENYL_TRACE(LOGGER, "Button released!");
        isPressed = false;
    }
}

void UIButtonNode::onMousePosChange (glm::vec2 oldMousePos) {
    if (getMousePos().x >= 0 && getMousePos().y >= 0 && getMousePos().x <= size.x && getMousePos().y <= size.y) {
        isHovered = true;
    } else {
        isHovered = false;
    }
}

void UIButtonNode::onThemeUpdate () {
    minSize = getTheme().getProperty<glm::vec2>("size").orElse({0, 0});
    bgColour = getTheme().getProperty<glm::vec4>("color").orElse({0.0f, 0.0f, 0.0f, 1.0f});
    hoverColour = getTheme().getProperty<glm::vec4>("hover_color").orElse({0.0f, 0.0f, 0.0f, 1.0f});
    pressColour = getTheme().getProperty<glm::vec4>("press_color").orElse({0.0f, 0.0f, 0.0f, 1.0f});
}

bool UIButtonNode::isDown () const {
    return isPressed;
}
