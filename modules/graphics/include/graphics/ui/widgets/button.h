#pragma once

#include "container.h"
#include "widget.h"
#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    struct ButtonPressEvent {};

    class ButtonWidget : public Widget {
    private:
        enum class ButtonState {
            UNPRESSED,
            HOVERED,
            PRESSED,
            PRESSED_UNHOVER
        };
        ContainerWidget container;
        ButtonState state = ButtonState::UNPRESSED;

        glm::vec4 widgetDefaultBgColor{};
        glm::vec4 widgetHoverBgColor{};
        glm::vec4 widgetPressBgColor{};

        void onHover ();
        void onUnhover ();
        void onPress ();
        void onRelease ();
    public:
        explicit ButtonWidget (const Modifier& modifier = {});

        [[nodiscard]] Widget* child () const noexcept {
            return container.child();
        }

        Widget* replace (std::unique_ptr<Widget> newChild);

        template <std::derived_from<Widget> T>
        T* replace (T&& widget) {
            return static_cast<T*>(replace(std::make_unique<T>(std::forward<T>(widget))));
        }

        template <std::derived_from<Widget> T, typename ...Args> requires std::constructible_from<T, Args&&...>
        T* emplace (Args&&... args) {
            return static_cast<T*>(replace(std::make_unique<T>(std::forward<Args>(args)...)));
        }

        glm::vec4 defaultBgColor () const noexcept {
            return widgetDefaultBgColor;
        }
        void setDefaultBgColor (glm::vec4 color);

        glm::vec4 hoverBgColor () const noexcept {
            return widgetDefaultBgColor;
        }
        void setHoverBgColor (glm::vec4 color);

        glm::vec4 pressBgColor () const noexcept {
            return widgetPressBgColor;
        }
        void setPressBgColor (glm::vec4 color);

        void measure (const WidgetConstraints& constraints) override;
        void render (Canvas& canvas) override;
        bool pointerUpdate (glm::vec2 pointer) override;
        void pointerLeave () override;
    };
}
