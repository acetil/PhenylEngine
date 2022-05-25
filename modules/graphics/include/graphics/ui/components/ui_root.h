#pragma once

#include <utility>
#include <vector>

#include "ui_component.h"

namespace graphics::ui {
    class UIRootNode : public UIComponentNode {
    private:
        std::vector<std::tuple<glm::vec2, glm::vec2, std::shared_ptr<UIComponentNode>>> childNodes;
    public:
        UIRootNode () : UIComponentNode("default") {}

        void render(UIManager &uiManager) override;
        UIAnchor getAnchor() override;

        void addChildNode (const std::shared_ptr<UIComponentNode>& child, glm::vec2 pos);

        void onMousePosChange(glm::vec2 oldMousePos) override;

        bool onMousePress() override;
        void onMouseRelease() override;

        void onThemeUpdate(Theme *theme) override;
    };
}