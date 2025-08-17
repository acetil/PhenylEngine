#pragma once

#include "component.h"
#include "graphics/font/font.h"
#include "ui/widgets/widget.h"

#include <string>

namespace phenyl::graphics {
struct UILabelProps {
    std::string text;
    std::uint32_t textSize = 12;
    std::shared_ptr<Font> font = {};
    glm::vec3 color = {1.0f, 1.0f, 1.0f};

    Modifier modifier = {};
};

class UILabelComponent : public UIComponent<UILabelComponent, UILabelProps> {
public:
    UILabelComponent (UI& ui, UILabelProps props) : UIComponent{ui, std::move(props)} {}

    void render (UI& ui) override;
};
} // namespace phenyl::graphics
