#include "ui/reactive/node.h"

using namespace phenyl::graphics;

UINode::UINode (Modifier modifier) : m_modifier{std::move(modifier)} {}

UINode* UINode::pick (glm::vec2 pointer) noexcept {
    return pointer.x >= 0.0f && pointer.x < dimensions().x //
            && pointer.y >= 0.0f && pointer.y < dimensions().y ?
        this :
        nullptr;
}

bool UINode::doPointerUpdate (glm::vec2 pointer) {
    PHENYL_DASSERT(m_state);
    if (pointer.x < 0.0f || pointer.y < 0.0f || pointer.x >= dimensions().x || pointer.y >= dimensions().y) {
        if (m_state->oldPointerPos) {
            handle(UIEvent{MouseExitEvent{}});
            m_state->oldPointerPos = std::nullopt;
        }
        return false;
    }

    if (m_state->oldPointerPos) {
        if (*m_state->oldPointerPos != pointer) {
            handle(UIEvent{MouseMoveEvent{*m_state->oldPointerPos, pointer}});
            m_state->oldPointerPos = pointer;
        }
    } else {
        handle(UIEvent{MouseEnterEvent{pointer}});
        m_state->oldPointerPos = pointer;
    }
    return true;
}

void UINode::onPointerLeave () {
    PHENYL_DASSERT(m_state);
    if (m_state->oldPointerPos) {
        handle(UIEvent{MouseExitEvent{}});
        m_state->oldPointerPos = std::nullopt;
    }
}

bool UINode::raise (const UIEvent& event) {
    return bubbleUp(event);
}

void UINode::measure (const WidgetConstraints& constraints) {
    setDimensions(modifier().minSize);
}

void UINode::setDimensions (glm::vec2 dimensions) {
    m_dimensions = dimensions;
}

void UINode::setParent (UINode* parent) {
    m_parent = parent;
}

void UINode::setState (detail::UINodeState* state) {
    m_state = state;
}

bool UINode::handle (const UIEvent& event) {
    for (const auto& handler : modifier().handlers) {
        if (handler(event)) {
            return true;
        }
    }

    return false;
}

bool UINode::bubbleUp (const UIEvent& event) {
    if (handle(event)) {
        return true;
    }

    return m_parent ? m_parent->bubbleUp(event) : false;
}
