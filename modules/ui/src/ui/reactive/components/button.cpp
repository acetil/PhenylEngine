#include "ui/reactive/components/button.h"

#include "ui/reactive/components/container.h"
#include "ui/reactive/node.h"
#include "ui/reactive/ui.h"

using namespace phenyl::graphics;

UIButtonComponent::UIButtonComponent (UI& ui, UIButtonProps props) : UIComponent{ui, std::move(props)} {
    m_state = makeState(ButtonState::UNPRESSED);
}

UIRenderResult UIButtonComponent::render (UIContext& ctx) const {
    auto modifier = props().modifier;
    modifier = modifier.handle([this] (const MouseEnterEvent&) { onHover(); })
                   .handle([this] (const MouseExitEvent&) { onUnhover(); })
                   .handle([this] (const MousePressEvent&) {
                       onPress();
                       return true;
                   })
                   .handle([this] (const MouseReleaseEvent&) {
                       onRelease();
                       return true;
                   });
    return ctx.render<UIContainerComponent>(UIContainerProps{
      .bgColor = GetBgColor(props(), *m_state),
      .modifier = std::move(modifier),
      .child = [this] (UIContext& ctx) { return props().child(ctx); },
    });
}

glm::vec4 UIButtonComponent::GetBgColor (const UIButtonProps& props, ButtonState state) {
    switch (state) {
    case ButtonState::UNPRESSED:
        return props.defaultBgColor;
    case ButtonState::HOVERED:
        return props.getHoverColor();
    case ButtonState::PRESSED:
    case ButtonState::PRESSED_UNHOVER:
        return props.getPressColor();
    }

    PHENYL_ABORT("Unexpected value for ButtonState: {}", static_cast<std::uint32_t>(state));
}

void UIButtonComponent::onHover () const {
    m_state.set(ButtonState::HOVERED);
}

void UIButtonComponent::onUnhover () const {
    m_state.update([] (ButtonState& state) {
        if (state == ButtonState::PRESSED) {
            state = ButtonState::PRESSED_UNHOVER;
        } else {
            state = ButtonState::UNPRESSED;
        }
    });
}

void UIButtonComponent::onPress () const {
    m_state.set(ButtonState::PRESSED);
}

void UIButtonComponent::onRelease () const {
    auto currState = *m_state;

    m_state.update([] (ButtonState& state) {
        if (state == ButtonState::PRESSED) {
            state = ButtonState::HOVERED;
        } else {
            state = ButtonState::UNPRESSED;
        }
    });
    if (currState == ButtonState::PRESSED) {
        props().onPress();
    }
}
