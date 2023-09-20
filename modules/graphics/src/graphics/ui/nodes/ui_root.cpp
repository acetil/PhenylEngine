#include "graphics/ui/nodes/ui_root.h"
#include "graphics/ui/ui_manager.h"

using namespace phenyl::graphics::ui;

void UIRootNode::render (UIManager& uiManager) {
    lockChildDestruction();
    for (auto& i : childNodes) {
        uiManager.pushOffset(std::get<0>(i));
        std::get<std::shared_ptr<UIComponentNode>>(i)->render(uiManager);
        uiManager.popOffset();
    }
    unlockChildDestruction();
}

UIAnchor UIRootNode::getAnchor () {
    return {};
}

void UIRootNode::addChildNode (const std::shared_ptr<UIComponentNode>& child, glm::vec2 pos) {
    auto anchor = child->getAnchor();
    // TODO: deal with anchor

    glm::vec2 size = anchor.minimumSize;

    if (anchor.maximumSize.x != -1) {
        size.x = anchor.maximumSize.x;
    }
    if (anchor.maximumSize.y != -1) {
        size.y = anchor.maximumSize.y;
    }

    childNodes.emplace_back(pos, size, child);
    child->setSize(size);
    addChild(child);
}

void UIRootNode::onMousePosChange (glm::vec2 oldMousePos) {
    lockChildDestruction();
    for (auto& i : childNodes) {
        std::get<std::shared_ptr<UIComponentNode>>(i)->setMousePos(getMousePos() - std::get<0>(i));
    }
}

bool UIRootNode::onMousePress () {
    lockChildDestruction();
    for (auto& i : childNodes) {
        auto b = std::get<std::shared_ptr<UIComponentNode>>(i)->onMousePress();
        if (b) {
            unlockChildDestruction();
            return true;
        }
    }
    unlockChildDestruction();
    return false;
}

void UIRootNode::onMouseRelease () {
    lockChildDestruction();
    for (auto& i : childNodes) {
        std::get<std::shared_ptr<UIComponentNode>>(i)->onMouseRelease();
    }
    unlockChildDestruction();
}

void UIRootNode::onThemeUpdate (Theme* theme) {
    lockChildDestruction();
    for (auto& i : childNodes) {
        auto& child = std::get<2>(i);
        child->applyTheme(theme);

        auto anchor = child->getAnchor();
        // TODO: deal with anchor

        glm::vec2 size = anchor.minimumSize;

        if (anchor.maximumSize.x != -1) {
            size.x = anchor.maximumSize.x;
        }
        if (anchor.maximumSize.y != -1) {
            size.y = anchor.maximumSize.y;
        }

        child->setSize(size);
    }
    unlockChildDestruction();
}

void UIRootNode::destroyChild (UIComponentNode* childNode) {
    childNodes.erase(std::remove_if(childNodes.begin(), childNodes.end(), [childNode] (auto& i) {
        return std::get<2>(i).get() == childNode;
    }), childNodes.end());
}
