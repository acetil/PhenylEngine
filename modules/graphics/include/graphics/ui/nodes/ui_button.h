#pragma once

#include <utility>

#include "ui_node.h"

namespace graphics::ui {
    class UIButtonNode : public UIComponentNode {
    private:
        glm::vec2 minSize{};
        glm::vec4 bgColour{};
        glm::vec4 pressColour{};
        glm::vec4 hoverColour{};
        bool isPressed = false;
        bool isHovered = false;
    public:
        explicit UIButtonNode (const std::string& themeClass) : UIComponentNode(themeClass) {}

        void render(UIManager &uiManager) override;
        UIAnchor getAnchor() override;
        void onMousePosChange(glm::vec2 oldMousePos) override;
        bool onMousePress() override;
        void onMouseRelease() override;
        void onThemeUpdate(Theme *theme) override;

        [[nodiscard]] bool isDown () const;
    };
}