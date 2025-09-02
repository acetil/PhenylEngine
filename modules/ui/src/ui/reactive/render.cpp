#include "ui/reactive/render.h"

#include "concrete_ui.h"

using namespace phenyl::graphics;

UIRenderResult::Builder& UIRenderResult::Builder::withChild (UIRenderResult result) {
    PHENYL_DASSERT(m_node);
    if (result.m_node) {
        m_node->addChild(std::move(result.m_node));
    }
    return *this;
}

UIRenderResult UIRenderResult::Builder::build () {
    return UIRenderResult{std::move(m_node)};
}

UIRenderResult::Builder::Builder (std::unique_ptr<UINode> node) : m_node{std::move(node)} {
    PHENYL_DASSERT(m_node);
}

std::unique_ptr<UINode> UIRenderResult::node () && {
    return std::move(m_node);
}

UIRenderResult::UIRenderResult (std::unique_ptr<UINode> node) : m_node{std::move(node)} {}

UIContext::UIContext (ConcreteUI& ui, UIComponentNode* node) : m_ui{ui}, m_current{node} {}

UIRenderResult UIContext::renderComponent () {
    PHENYL_DASSERT(m_current);
    auto result = m_current->component->render(*this);

    std::erase_if(m_current->children,
        [&] (const auto& item) { return !m_current->seenChildren.contains(item.first); });
    m_current->seenChildren.clear();
    m_current->node = result.m_node.get();

    m_ui.onComponentRender(m_current->component.get());

    return result;
}

std::size_t UIContext::childrenCount () const {
    PHENYL_DASSERT(m_current);
    return m_current->seenChildren.size();
}

UIComponentBase* UIContext::getChild (std::size_t key) const {
    PHENYL_DASSERT(m_current);
    auto it = m_current->children.find(key);
    return it != m_current->children.end() ? it->second.component.get() : nullptr;
}

UIComponentBase* UIContext::setChild (std::size_t key, std::unique_ptr<UIComponentBase> component) {
    PHENYL_DASSERT(component);
    PHENYL_DASSERT(m_current);

    auto* ptr = component.get();
    auto it =
        m_current->children
            .emplace(key,
                UIComponentNode{.component = std::move(component), .parent = m_current, .depth = m_current->depth + 1})
            .first;
    m_ui.insertComponent(&it->second);
    return ptr;
}

UIRenderResult UIContext::renderChild (std::size_t key, UIComponentBase* component) {
    PHENYL_DASSERT(component);
    PHENYL_DASSERT(m_current);
    auto it = m_current->children.find(key);
    PHENYL_DASSERT(it != m_current->children.end());
    m_current->seenChildren.emplace(key);

    return UIContext{m_ui, &it->second}.renderComponent();
}

void UIContext::setNode (UINode* node) {
    PHENYL_DASSERT(m_current);
    PHENYL_DASSERT(node);
    m_ui.insertNode(m_current->component.get(), node);
}

UIRenderResult phenyl::graphics::EmptyUIFactory (UIContext& ctx) {
    return ctx.makeEmptyNode();
}
