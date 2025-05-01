#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "widget.h"

namespace phenyl::graphics {
    class RootWidget : public Widget {
    public:
        RootWidget ();

        Widget* insert (std::unique_ptr<Widget> widget);
        template <std::derived_from<Widget> T>
        T* insert (T&& widget) {
            return static_cast<T*>(insert(std::make_unique<T>(std::forward<T>(widget))));
        }

        template <std::derived_from<Widget> T, typename ...Args> requires std::constructible_from<T, Args&&...>
        T* emplace (Args&&... args) {
            return static_cast<T*>(insert(std::make_unique<T>(std::forward<Args>(args)...)));
        }

        Widget* pick (glm::vec2 pointer) noexcept override;
        void measure (const WidgetConstraints& constraints) override;
        void update () override;
        void render (Canvas& canvas) override;
        void queueChildDestroy (Widget* child) override;
        bool pointerUpdate (glm::vec2 pointer) override;
        void pointerLeave () override;

    private:
        std::vector<std::unique_ptr<Widget>> m_children;
        std::unordered_set<Widget*> m_widgetsToDelete;
    };
}