#pragma once

#include "ui_component.h"
#include "graphics/ui/nodes/ui_button.h"

namespace phenyl::graphics::ui {

    class UIButtonNode;

    class UIButton : public UIComponent<UIButtonNode> {
    private:
    public:
        explicit UIButton (const std::string&  className);

        operator bool ();
        bool isDown ();
    };
}