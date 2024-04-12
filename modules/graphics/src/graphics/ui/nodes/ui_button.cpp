#include "graphics/ui/nodes/ui_button.h"
#include "graphics/ui/ui_manager.h"
#include "logging/logging.h"

using namespace phenyl::graphics::ui;

static phenyl::Logger LOGGER{"UI_BUTTON", phenyl::graphics::detail::GRAPHICS_LOGGER};

void UIButtonNode::render (Canvas& canvas) {
    CanvasRoundedRect rect{
        .rect = CanvasRect{
            .size = size
        },
        .rounding = 10.0f
    };
    CanvasStyle style{
        .fill = CanvasFill::FILLED,
        .useAA = true
    };

    if (isPressed) {
        style.colour = pressColour;
    } else if (isHovered) {
        style.colour = hoverColour;
    } else {
        style.colour = bgColour;
    }

    canvas.render(rect, style);
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
