#pragma once
#include "ui.h"

#include <functional>

namespace phenyl::graphics {
namespace detail {
    struct UIRootChild {
        std::function<void(UI&, std::size_t)> comp;
        std::size_t id;
        std::size_t priority;
    };

    class UIRootComponents {
    public:
        using iterator = std::vector<UIRootChild>::iterator;
        using const_iterator = std::vector<UIRootChild>::const_iterator;

        std::size_t insert (std::size_t priority, std::function<void(UI&, std::size_t)> comp);
        void remove (std::size_t id);

        iterator begin () noexcept;
        iterator end () noexcept;

        const_iterator begin () const noexcept;
        const_iterator cbegin () const noexcept;

        const_iterator end () const noexcept;
        const_iterator cend () const noexcept;

    private:
        std::vector<UIRootChild> m_children;
        std::size_t m_nextId = 1;
    };
} // namespace detail

class UIRoot {
public:
    explicit UIRoot (UIAtom<detail::UIRootComponents> comps);

    template <typename T, std::copyable Props>
    requires UIComponentType<T, Props>
    std::size_t add (Props&& props) {
        return addComponent(0,
            [props = std::forward<Props>(props)] (UI& ui, std::size_t id) { ui.render<T>(id, props); });
    }

    template <typename T, std::copyable Props>
    requires UIComponentType<T, Props>
    std::size_t add (std::size_t priority, Props&& props) {
        return addComponent(priority,
            [props = std::forward<Props>(props)] (UI& ui, std::size_t id) { ui.render<T>(id, props); });
    }

    void remove (std::size_t id);

private:
    UIAtom<detail::UIRootComponents> m_rootComponents;

    std::size_t addComponent (std::size_t priority, std::function<void(UI&, std::size_t)> comp);
};
} // namespace phenyl::graphics
