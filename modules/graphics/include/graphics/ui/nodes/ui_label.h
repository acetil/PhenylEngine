#pragma once

#include <utility>

#include "common/assets/asset.h"
#include "ui_node.h"
#include "graphics/font/font.h"

namespace phenyl::graphics::ui {
    class UILabelNode : public UIComponentNode {
    private:
        std::string text;
        common::Asset<Font> font;
        int textSize = 12;
        glm::vec4 colour = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec2 labelSize{0, 0};
        bool doDebugRender = false;
    public:
        explicit UILabelNode (const std::string& themeClass);
        void render (Canvas& canvas) override;
        UIAnchor getAnchor() override;

        void setText (std::string&& newText);
        void setFont (const std::string& newFont);
        void setTextSize (int newTextSize);
        void setColour (glm::vec4 newColour);
        void setDebug (bool isDebug);

        void onThemeUpdate() override;

        const std::string& getCurrentText ();
    };
}