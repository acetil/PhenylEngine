#pragma once

#include "container.h"
#include "graphics/maths_headers.h"
#include "widget.h"

namespace phenyl::graphics {
struct ButtonPressEvent {};

class ButtonWidget : public Widget {
public:
    explicit ButtonWidget (const Modifier& modifier = {});

    [[nodiscard]] Widget* child () const noexcept {
        return m_container.child();
    }

    Widget* replace (std::unique_ptr<Widget> newChild);

    template<std::derived_from<Widget> T>
    T* replace (T&& widget) {
        return static_cast<T*>(replace(std::make_unique<T>(std::forward<T>(widget))));
    }

    template<std::derived_from<Widget> T, typename... Args>
    requires std::constructible_from<T, Args&&...>
    T* emplace (Args&&... args) {
        return static_cast<T*>(replace(std::make_unique<T>(std::forward<Args>(args)...)));
    }

    glm::vec4 defaultBgColor () const noexcept {
        return m_defaultBgColor;
    }

    void setDefaultBgColor (glm::vec4 color);

    glm::vec4 hoverBgColor () const noexcept {
        return m_defaultBgColor;
    }

    void setHoverBgColor (glm::vec4 color);

    glm::vec4 pressBgColor () const noexcept {
        return m_pressBgColor;
    }

    void setPressBgColor (glm::vec4 color);

    void measure (const WidgetConstraints& constraints) override;
    void render (Canvas& canvas) override;
    bool pointerUpdate (glm::vec2 pointer) override;
    void pointerLeave () override;

private:
    enum class ButtonState {
        UNPRESSED,
        HOVERED,
        PRESSED,
        PRESSED_UNHOVER
    };
    ContainerWidget m_container;
    ButtonState m_state = ButtonState::UNPRESSED;

    glm::vec4 m_defaultBgColor{};
    glm::vec4 m_hoverBgColor{};
    glm::vec4 m_pressBgColor{};

    void onHover ();
    void onUnhover ();
    void onPress ();
    void onRelease ();
};
} // namespace phenyl::graphics
