#pragma once

#include <utility>

#include "ui_component.h"

namespace graphics::ui {
    class UIButtonNode : public UIComponentNode {
    private:
        glm::vec2 minSize;
        glm::vec4 bgColour;
        glm::vec4 pressColour;
        glm::vec4 hoverColour;
        bool isPressed = false;
        bool isHovered = false;
    public:
        UIButtonNode (const std::string& themeClass, glm::vec2 size, glm::vec4 _bgColour, glm::vec4 _hoverColour, glm::vec4 _pressColour) : UIComponentNode(themeClass),
                minSize{size}, bgColour{_bgColour}, pressColour{_pressColour}, hoverColour{_hoverColour} {}

        void render(UIManager &uiManager) override;
        UIAnchor getAnchor() override;
        void onMousePosChange(glm::vec2 oldMousePos) override;
        bool onMousePress() override;
        void onMouseRelease() override;
        void onThemeUpdate(Theme *theme) override;
    };
}