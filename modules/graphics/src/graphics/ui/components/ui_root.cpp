#include "graphics/ui/components/ui_root.h"
#include "graphics/ui/ui_manager.h"

using namespace graphics::ui;

void UIRootNode::render (UIManager& uiManager) {
    for (auto& i : childNodes) {
        uiManager.pushOffset(std::get<0>(i));
        std::get<std::shared_ptr<UIComponentNode>>(i)->render(uiManager);
        uiManager.popOffset();
    }
}

UIAnchor UIRootNode::getAnchor () {
    return {};
}

void UIRootNode::addChildNode (const std::shared_ptr<UIComponentNode>& child, glm::vec2 pos) {
    auto anchor = child->getAnchor();
    // TODO: deal with anchor

    childNodes.emplace_back(pos, anchor.minimumSize, child);
    child->setSize(anchor.minimumSize);
}

void UIRootNode::onMousePosChange (glm::vec2 oldMousePos) {
    for (auto& i : childNodes) {
        std::get<std::shared_ptr<UIComponentNode>>(i)->setMousePos(getMousePos() - std::get<0>(i));
    }
}

bool UIRootNode::onMousePress () {
    for (auto& i : childNodes) {
        auto b = std::get<std::shared_ptr<UIComponentNode>>(i)->onMousePress();
        if (b) {
            return true;
        }
    }

    return false;
}

void UIRootNode::onMouseRelease () {
    for (auto& i : childNodes) {
        std::get<std::shared_ptr<UIComponentNode>>(i)->onMouseRelease();
    }
}
