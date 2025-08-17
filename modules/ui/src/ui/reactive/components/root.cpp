#include "ui/reactive/components/root.h"

#include <algorithm>

using namespace phenyl::graphics;

std::size_t detail::UIRootComponents::insert (std::size_t priority, std::function<void(UI&, std::size_t)> comp) {
    auto id = m_nextId++;
    m_children.emplace_back(std::move(comp), id, priority);
    std::ranges::sort(m_children, [] (const auto& lhs, const auto& rhs) {
        return lhs.priority != rhs.priority ? lhs.priority < rhs.priority : lhs.id < rhs.id;
    });

    return id;
}

void detail::UIRootComponents::remove (std::size_t id) {
    std::erase_if(m_children, [&] (const auto& item) { return item.id == id; });
}

detail::UIRootComponents::iterator detail::UIRootComponents::begin () noexcept {
    return m_children.begin();
}

detail::UIRootComponents::iterator detail::UIRootComponents::end () noexcept {
    return m_children.end();
}

detail::UIRootComponents::const_iterator detail::UIRootComponents::begin () const noexcept {
    return cbegin();
}

detail::UIRootComponents::const_iterator detail::UIRootComponents::cbegin () const noexcept {
    return m_children.cbegin();
}

detail::UIRootComponents::const_iterator detail::UIRootComponents::end () const noexcept {
    return cend();
}

detail::UIRootComponents::const_iterator detail::UIRootComponents::cend () const noexcept {
    return m_children.cend();
}

UIRoot::UIRoot (UIAtom<detail::UIRootComponents> comps) : m_rootComponents{std::move(comps)} {}

void UIRoot::remove (std::size_t id) {
    m_rootComponents.update([&] (detail::UIRootComponents& comps) { comps.remove(id); });
}

std::size_t UIRoot::addComponent (std::size_t priority, std::function<void(UI&, std::size_t)> comp) {
    std::size_t id;
    m_rootComponents.update([&] (detail::UIRootComponents& comps) { id = comps.insert(priority, std::move(comp)); });
    return id;
}
