#pragma once

#include "graphics/font/font.h"
#include "ui/reactive/component.h"

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

    UIRenderResult render (UIContext& ctx) const override;
};

struct UIDynamicLabelProps {
    UIAtom<std::string> text;
    std::uint32_t textSize = 12;
    std::shared_ptr<Font> font;
    glm::vec3 color = {1.0f, 1.0f, 1.0f};

    Modifier modifier = {};
};

class UIDynamicLabelComponent : public UIComponent<UIDynamicLabelComponent, UIDynamicLabelProps> {
public:
    UIDynamicLabelComponent (UI& ui, UIDynamicLabelProps labelProps);

    UIRenderResult render (UIContext& ctx) const override;
};
} // namespace phenyl::graphics
