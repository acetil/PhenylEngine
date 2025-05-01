#include "graphics/ui/widgets/widget.h"

#include "graphics/ui/widgets/event.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

Widget::Widget (const Modifier& modifier) : m_modifier{modifier} {}
Widget::~Widget () = default;

Widget* Widget::pick (glm::vec2 pointer) noexcept {
    return pointer.x >= 0.0f && pointer.x < dimensions().x && pointer.y >= 0.0f && pointer.y < dimensions().y ? this : nullptr;
}

void Widget::measure (const WidgetConstraints& constraints) {
    float maxWidth = modifier().maxWidth ? std::min(*modifier().maxWidth, constraints.maxSize.x) : constraints.maxSize.x;
    float maxHeight = modifier().maxHeight ? std::min(*modifier().maxHeight, constraints.maxSize.y) : constraints.maxSize.y;
    setDimensions(glm::vec2{maxWidth, maxHeight});
}

void Widget::update () {}

void Widget::queueDestroy () {
    PHENYL_ASSERT_MSG(m_parent, "Attempted to queue destruction on unparented widget!");
    m_parent->queueChildDestroy(this);
}

void Widget::setParent (Widget* parent) {
    m_parent = parent;
}

bool Widget::pointerUpdate (glm::vec2 pointer) {
    if (pointer.x < 0.0f || pointer.y < 0.0f || pointer.x >= dimensions().x || pointer.y >= dimensions().y) {
        if (m_oldPointerPos) {
            handle(UIEvent{MouseExitEvent{}});
            m_oldPointerPos = std::nullopt;
        }
        return false;
    }

    if (m_oldPointerPos) {
        if (*m_oldPointerPos != pointer) {
            handle(UIEvent{MouseMoveEvent{*m_oldPointerPos, pointer}});
            m_oldPointerPos = pointer;
        }
    } else {
        handle(UIEvent{MouseEnterEvent{pointer}});
        m_oldPointerPos = pointer;
    }
    return true;
}

void Widget::pointerLeave () {
    if (m_oldPointerPos) {
        handle(UIEvent{MouseExitEvent{}});
        m_oldPointerPos = std::nullopt;
    }
}

void Widget::raise (const UIEvent& event) {
    bubbleUp(event);
}


bool Widget::bubbleUp (const UIEvent& event) {
    if (handle(event)) {
        return true;
    }

    return parent() ? parent()->bubbleUp(event) : false;
}

bool Widget::handle (const UIEvent& event) {
    auto it = m_listeners.find(event.type());
    if (it == m_listeners.end()) {
        return false;
    }

    return it->second->handle(event);
}

void Widget::queueChildDestroy (Widget* child) {}

void Widget::setModifier (const Modifier& modifier) {
    m_modifier = modifier;
}

void Widget::setDimensions (glm::vec2 newDims) {
    m_size = newDims;
}

void Widget::setOffset (glm::vec2 newOffset) {
    m_offset = newOffset;
}
