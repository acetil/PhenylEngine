#include "ui/widgets/button.h"

using namespace phenyl::graphics;

ButtonWidget::ButtonWidget (const Modifier& modifier) : Widget{modifier}, m_container{modifier} {
    m_container.setParent(this);
    addListener([this] (const MouseEnterEvent&) { onHover(); });

    addListener([this] (const MouseExitEvent&) { onUnhover(); });

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
    return m_container.replace(std::move(newChild));
}

void ButtonWidget::setDefaultBgColor (glm::vec4 color) {
    m_defaultBgColor = color;
    if (m_state == ButtonState::UNPRESSED) {
        m_container.setBgColor(m_defaultBgColor);
    }
}

void ButtonWidget::setHoverBgColor (glm::vec4 color) {
    m_hoverBgColor = color;
    if (m_state == ButtonState::HOVERED) {
        m_container.setBgColor(m_hoverBgColor);
    }
}

void ButtonWidget::setPressBgColor (glm::vec4 color) {
    m_pressBgColor = color;
    if (m_state == ButtonState::PRESSED || m_state == ButtonState::PRESSED_UNHOVER) {
        m_container.setBgColor(m_pressBgColor);
    }
}

void ButtonWidget::measure (const WidgetConstraints& constraints) {
    m_container.measure(constraints);
    setDimensions(m_container.dimensions());
}

void ButtonWidget::render (Canvas& canvas) {
    m_container.render(canvas);
}

bool ButtonWidget::pointerUpdate (glm::vec2 pointer) {
    m_container.pointerUpdate(pointer);
    return Widget::pointerUpdate(pointer);
}

void ButtonWidget::pointerLeave () {
    m_container.pointerLeave();
    Widget::pointerLeave();
}

void ButtonWidget::onHover () {
    switch (m_state) {
    case ButtonState::UNPRESSED:
        m_state = ButtonState::HOVERED;
        m_container.setBgColor(m_hoverBgColor);
        break;
    case ButtonState::PRESSED_UNHOVER:
        m_state = ButtonState::PRESSED;
        break;
    default:
        PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onHover on {}", static_cast<int>(m_state));
        break;
    }
}

void ButtonWidget::onUnhover () {
    switch (m_state) {
    case ButtonState::HOVERED:
        m_state = ButtonState::UNPRESSED;
        m_container.setBgColor(m_defaultBgColor);
        break;
    case ButtonState::PRESSED:
        m_state = ButtonState::PRESSED_UNHOVER;
        break;
    default:
        PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onUnhover on {}", static_cast<int>(m_state));
    }
}

void ButtonWidget::onPress () {
    switch (m_state) {
    case ButtonState::HOVERED:
        m_state = ButtonState::PRESSED;
        m_container.setBgColor(m_pressBgColor);
        break;
    default:
        PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onPress on {}", static_cast<int>(m_state));
    }
}

void ButtonWidget::onRelease () {
    switch (m_state) {
    case ButtonState::PRESSED:
        raise(UIEvent{ButtonPressEvent{}});
        m_state = ButtonState::HOVERED;
        m_container.setBgColor(m_hoverBgColor);
        break;
    case ButtonState::PRESSED_UNHOVER:
        m_state = ButtonState::UNPRESSED;
        m_container.setBgColor(m_defaultBgColor);
        break;
    default:
        PHENYL_DASSERT_MSG(false, "Disallowed state transition for button: onRelease on {}", static_cast<int>(m_state));
    }
}
