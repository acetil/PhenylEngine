#pragma once

#include "ui_component.h"
#include "graphics/ui/nodes/ui_flexbox.h"

namespace phenyl::graphics::ui {
    class UIFlexbox : public UIComponent<UIFlexBoxNode> {
    private:
        void addUINode (std::shared_ptr<UIComponentNode> node, int index);
    public:
        UIFlexbox (const std::string& themeClass) : UIComponent<UIFlexBoxNode>(std::make_shared<UIFlexBoxNode>(themeClass)) {}

        template <typename T>
        void add (UIComponent<T>& component, int index = -1) {
            addUINode(component.transferNode(), index);
        }

        void setAxes (Axis primaryAxis, Axis secondaryAxis);
    };
}