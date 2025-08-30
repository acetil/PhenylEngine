#pragma once

#include "graphics/maths_headers.h"
#include "ui/reactive/component.h"
#include "ui/widgets/widget.h"

namespace phenyl::graphics {
struct UIButtonProps {
    glm::vec4 defaultBgColor;
    std::optional<glm::vec4> hoverBgColor;
    std::optional<glm::vec4> pressBgColor;

    Modifier modifier;

    std::function<void()> onPress = [] () {
    };

    std::function<void(UI&)> child = [] (UI&) {
    };

    glm::vec4 getHoverColor () const noexcept {
        return hoverBgColor ? *hoverBgColor : defaultBgColor;
    }

    glm::vec4 getPressColor () const noexcept {
        return pressBgColor ? *pressBgColor : getHoverColor();
    }
};

class UIButtonComponent : public UIComponent<UIButtonComponent, UIButtonProps> {
public:
    UIButtonComponent (UI& ui, UIButtonProps props);

    void render (UI& ui) const override;

private:
    enum class ButtonState {
        UNPRESSED,
        HOVERED,
        PRESSED,
        PRESSED_UNHOVER
    };

    static glm::vec4 GetBgColor (const UIButtonProps& props, ButtonState state);

    UIState<ButtonState> m_state{};

    void onHover () const;
    void onUnhover () const;
    void onPress () const;
    void onRelease () const;
};
} // namespace phenyl::graphics
