#pragma once

#include <utility>

#include "ui_node.h"

namespace phenyl::graphics::ui {
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

        void render(Canvas& canvas) override;
        UIAnchor getAnchor() override;
        void onMousePosChange(glm::vec2 oldMousePos) override;
        bool onMousePress() override;
        void onMouseRelease() override;
        void onThemeUpdate() override;

        [[nodiscard]] bool isDown () const;
    };
}