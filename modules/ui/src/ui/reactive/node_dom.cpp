#include "node_dom.h"

using namespace phenyl::graphics;

UINode* UINodeDOM::pick (glm::vec2 pointer) const {
    return m_root->pick(pointer);
}

void UINodeDOM::pointerUpdate (glm::vec2 pointer) {
    m_root->doPointerUpdate(pointer);
}

void UINodeDOM::measure (const WidgetConstraints& constraints) {
    PHENYL_DASSERT(m_root);
    m_root->measure(constraints);
}

void UINodeDOM::render (Canvas& canvas) {
    m_root->render(canvas);
}

void UINodeDOM::insert (std::size_t id, UINode* node) {
    m_nodes[id] = node;
    auto it = m_nodeStates.emplace(id, detail::UINodeState{.id = id}).first;
    node->setState(&it->second);
}

void UINodeDOM::remove (std::size_t id) {
    m_nodes.erase(id);
    m_nodeStates.erase(id);
}

void UINodeDOM::setRoot (std::unique_ptr<UINode> root) {
    m_root = std::move(root);
}

UINode* UINodeDOM::get (std::size_t id) const {
    auto it = m_nodes.find(id);
    return it != m_nodes.end() ? it->second : nullptr;
}

void UINodeDOM::clear () {
    m_root = nullptr;
    m_nodes.clear();
}
