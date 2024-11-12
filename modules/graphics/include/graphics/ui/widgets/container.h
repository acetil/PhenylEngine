#pragma once

#include <memory>

#include "graphics/maths_headers.h"

#include "widget.h"

namespace phenyl::graphics {
    class ContainerWidget : public Widget {
    private:
        std::unique_ptr<Widget> childWidget;
        glm::vec4 widgetBgColour{};
        glm::vec4 widgetBorderColour{};
        float widgetBorderSize = 0.0f;

    public:
        explicit ContainerWidget (std::unique_ptr<Widget> childWidget = nullptr, const Modifier& modifier = {});
        explicit ContainerWidget (const Modifier& modifier) : ContainerWidget{nullptr, modifier} {}

        template <std::derived_from<Widget> T>
        explicit ContainerWidget (T&& widget, const Modifier& modifier = {}) : ContainerWidget{std::make_unique<T>(std::forward<T>(widget)), modifier} {}

        Widget* child () const noexcept {
            return childWidget.get();
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

        glm::vec4 bgColor () const noexcept {
            return widgetBgColour;
        }
        void setBgColor (glm::vec4 newBgColor);

        glm::vec4 borderColor () const noexcept {
            return widgetBorderColour;
        }
        void setBorderColor (glm::vec4 newBorderColor);

        float borderSize () const noexcept {
            return widgetBorderSize;
        }
        void setBorderSize (float newBorderSize);

        void setOffset (glm::vec2 offset) override;
        void update () override;
        void measure (const WidgetConstraints& constraints) override;
        void render (Canvas& canvas) override;
        bool pointerUpdate (glm::vec2 pointer) override;
        void pointerLeave () override;
    };
}
