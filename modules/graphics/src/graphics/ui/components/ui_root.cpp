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

    glm::vec2 size = anchor.minimumSize;

    if (anchor.maximumSize.x != -1) {
        size.x = anchor.maximumSize.x;
    }
    if (anchor.maximumSize.y != -1) {
        size.y = anchor.maximumSize.y;
    }

    childNodes.emplace_back(pos, size, child);
    child->setSize(size);
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

void UIRootNode::onThemeUpdate (Theme* theme) {
    for (auto& i : childNodes) {
        std::get<2>(i)->applyTheme(theme);
    }
}
