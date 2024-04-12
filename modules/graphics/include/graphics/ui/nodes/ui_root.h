#pragma once

#include <utility>
#include <vector>

#include "ui_node.h"
#include "ui_container.h"

namespace phenyl::graphics::ui {
    class UIRootNode : public UIContainerNode {
    private:
        std::vector<std::tuple<glm::vec2, glm::vec2, std::shared_ptr<UIComponentNode>>> childNodes;
    public:
        UIRootNode () : UIContainerNode("default") {}

        void render (Canvas& canvas) override;
        UIAnchor getAnchor() override;

        void addChildNode (const std::shared_ptr<UIComponentNode>& child, glm::vec2 pos);

        void onMousePosChange(glm::vec2 oldMousePos) override;

        bool onMousePress() override;
        void onMouseRelease() override;

        void onThemeUpdate() override;

        void destroyChild(UIComponentNode *childNode) override;
    };
}