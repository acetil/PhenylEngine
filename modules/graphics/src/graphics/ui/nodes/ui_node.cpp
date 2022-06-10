#include "graphics/ui/nodes/ui_node.h"
#include "graphics/ui/nodes/ui_container.h"

using namespace graphics::ui;

void UIComponentNode::queueDestroy () {
    // TODO: queue destroy from parent
    onDestroyRequest();

    if (parent != nullptr) {
        parent->queueChildDestroy(this);
    }
}

void UIComponentNode::setParent (UIContainerNode* _parent) {
    parent = _parent;
}

bool UIComponentNode::isDirty () {
    return dirty;
}

void UIComponentNode::clearDirty () {
    dirty = false;
}

void UIComponentNode::markDirty () {
    dirty = true;
}
