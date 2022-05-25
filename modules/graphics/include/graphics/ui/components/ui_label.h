#pragma once

#include <utility>

#include "ui_component.h"
#include "graphics/font/rendered_text.h"

namespace graphics::ui {
    class UILabelNode : public UIComponentNode {
    private:
        std::string text;
        std::string font = "noto-serif";
        int textSize = 12;
        glm::vec4 colour = {1.0f, 1.0f, 1.0f, 1.0f};
        RenderedText renderedText{0};
        bool doTextRender = false;
        bool doDebugRender = false;
    public:
        explicit UILabelNode (const std::string& themeClass) : UIComponentNode(themeClass) {}
        void render(UIManager &uiManager) override;
        UIAnchor getAnchor() override;

        void setText (std::string newText);
        void setFont (std::string newFont);
        void setTextSize (int newTextSize);
        void setColour (glm::vec4 newColour);
        void setDebug (bool isDebug);

        void onThemeUpdate(Theme *theme) override;
    };
}