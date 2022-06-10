#include "graphics/ui/nodes/ui_container.h"

using namespace graphics::ui;

void UIContainerNode::queueChildDestroy (UIComponentNode* childNode) {
    if (lockNum > 0) {
        destroyQueue.emplace_back(childNode);
    } else {
        destroyChild(childNode);
    }
}

void UIContainerNode::lockChildDestruction () {
    lockNum++;
}

void UIContainerNode::unlockChildDestruction () {
    lockNum--;

    if (lockNum == 0) {
        for (auto& i: destroyQueue) {
            destroyChild(i);
        }

        destroyQueue.clear();
    }
}