#include "graphics/ui/components/ui_button.h"
#include "graphics/ui/nodes/ui_button.h"

using namespace graphics::ui;

UIButton::UIButton (const std::string& _className) : UIComponent<UIButtonNode>(std::make_shared<UIButtonNode>(_className)) {}


bool UIButton::isDown () {
    auto node = getNode();
    return node && node->isDown();
}

UIButton::operator bool () {
    return isDown();
}