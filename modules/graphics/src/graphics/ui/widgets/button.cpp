#include "graphics/ui/widgets/button.h"

using namespace phenyl::graphics;

ButtonWidget::ButtonWidget (const Modifier& modifier) : Widget{modifier}, container{modifier} {
    container.setParent(this);
    addListener([this] (const MouseEnterEvent&) {
        onHover();
    });

    addListener([this] (const MouseExitEvent&) {
        onUnhover();
    });

    addListener([this] (const MousePressEvent&) {
        onPress();
        return true;
    });

    addListener([this] (const MouseReleaseEvent&) {
        onRelease();
        return true;
    });
}

Widget* ButtonWidget::replace (std::unique_ptr<Widget> newChild) {
    return container.replace(std::move(newChild));
}

void ButtonWidget::setDefaultBgColor (glm::vec4 color) {
    widgetDefaultBgColor = color;
    if (state == ButtonState::UNPRESSED) {
        container.setBgColor(widgetDefaultBgColor);
    }
}

void ButtonWidget::setHoverBgColor (glm::vec4 color) {
    widgetHoverBgColor = color;
    if (state == ButtonState::HOVERED) {
        container.setBgColor(widgetHoverBgColor);
    }
}

void ButtonWidget::setPressBgColor (glm::vec4 color) {
    widgetPressBgColor = color;
    if (state == ButtonState::PRESSED || state == ButtonState::PRESSED_UNHOVER) {
        container.setBgColor(widgetPressBgColor);
    }
}

void ButtonWidget::measure (const WidgetConstraints& constraints) {
    container.measure(constraints);
    setDimensions(container.dimensions());
}

void ButtonWidget::render (Canvas& canvas) {
    container.render(canvas);
}

bool ButtonWidget::pointerUpdate (glm::vec2 pointer) {
    container.pointerUpdate(pointer);
    return Widget::pointerUpdate(pointer);
}

void ButtonWidget::pointerLeave () {
    container.pointerLeave();
    Widget::pointerLeave();
}

void ButtonWidget::onHover () {
    switch (state) {
        case ButtonState::UNPRESSED:
            state = ButtonState::HOVERED;
            container.setBgColor(widgetHoverBgColor);
            break;
        case ButtonState::PRESSED_UNHOVER:
            state = ButtonState::PRESSED;
            break;
        default:
            PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onHover on {}", static_cast<int>(state));
            break;
    }
}

void ButtonWidget::onUnhover () {
    switch (state) {
        case ButtonState::HOVERED:
            state = ButtonState::UNPRESSED;
            container.setBgColor(widgetDefaultBgColor);
            break;
        case ButtonState::PRESSED:
            state = ButtonState::PRESSED_UNHOVER;
            break;
        default:
            PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onUnhover on {}", static_cast<int>(state));
    }
}

void ButtonWidget::onPress () {
    switch (state) {
        case ButtonState::HOVERED:
            state = ButtonState::PRESSED;
            container.setBgColor(widgetPressBgColor);
            break;
        default:
            PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onPress on {}", static_cast<int>(state));
    }
}

void ButtonWidget::onRelease () {
    switch (state) {
        case ButtonState::PRESSED:
            raise(UIEvent{ButtonPressEvent{}});
            state = ButtonState::HOVERED;
            container.setBgColor(widgetHoverBgColor);
            break;
        case ButtonState::PRESSED_UNHOVER:
            state = ButtonState::UNPRESSED;
            container.setBgColor(widgetDefaultBgColor);
            break;
        default:
            PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onRelease on {}", static_cast<int>(state));
    }
}


