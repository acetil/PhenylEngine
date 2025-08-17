#pragma once

#include "graphics/maths_headers.h"
#include "ui/reactive/component.h"
#include "ui/widgets/widget.h"

namespace phenyl::graphics {
struct UIContainerProps {
    glm::vec4 bgColor = {0, 0, 0, 0};
    glm::vec4 borderColor = {0, 0, 0, 0};
    float borderSize = 0.0f;

    Modifier modifier{};

    std::function<void(UI&)> child = [] (UI&) {
    };
};

class UIContainerComponent : public UIComponent<UIContainerComponent, UIContainerProps> {
public:
    UIContainerComponent (UI& ui, UIContainerProps props);

    void render (UI& ui) const override;
};
} // namespace phenyl::graphics
