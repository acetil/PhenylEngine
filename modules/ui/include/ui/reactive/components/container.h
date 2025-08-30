#pragma once

#include "graphics/maths_headers.h"
#include "ui/reactive/component.h"
#include "ui/reactive/render.h"
#include "ui/widgets/widget.h"

namespace phenyl::graphics {
struct UIContainerProps {
    glm::vec4 bgColor = {0, 0, 0, 0};
    glm::vec4 borderColor = {0, 0, 0, 0};
    float borderSize = 0.0f;

    Modifier modifier{};

    UIComponentFactory child = EmptyUIFactory;
};

class UIContainerComponent : public UIComponent<UIContainerComponent, UIContainerProps> {
public:
    UIContainerComponent (UI& ui, UIContainerProps props);

    UIRenderResult render (UIContext& ctx) const override;
};
} // namespace phenyl::graphics
