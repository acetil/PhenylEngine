#pragma once

#include "ui_component.h"
#include "graphics/ui/nodes/ui_label.h"
#include "util/callback_member.h"

namespace phenyl::graphics::ui {
    class UILabel : public UIComponent<UILabelNode> {
    private:
        void setText (std::string&& newText);
        const std::string& getText ();

    public:
        explicit UILabel (const std::string& themeClass) : UIComponent<UILabelNode>(std::make_shared<UILabelNode>(themeClass)), text{*getNode(), &UILabelNode::getCurrentText, &UILabelNode::setText} {}

        util::CallbackMember<std::string, UILabelNode> text;

        const std::string& tempGetText () {
            return getText();
        }

        void tempSetText (std::string&& newText) {
            setText(std::forward<std::string>(newText));
        }
    };
}