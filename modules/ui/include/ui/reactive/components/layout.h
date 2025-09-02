#pragma once
#include "ui/reactive/component.h"
#include "ui/reactive/render.h"
#include "ui/reactive/ui.h"

namespace phenyl::graphics {
enum class LayoutAxis {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct UILayoutProps {
    LayoutAxis axis = LayoutAxis::DOWN;
    LayoutArrangement arrangement = LayoutArrangement::START;
    LayoutAlignment alignment = LayoutAlignment::START;

    Modifier modifier{};

    std::vector<UIComponentFactory> children;
};

class UILayoutComponent : public UIComponent<UILayoutComponent, UILayoutProps> {
public:
    UILayoutComponent (UI& ui, UILayoutProps props);

    UIRenderResult render (UIContext& ctx) const override;
};

struct UIColumnProps {
    ColumnDirection direction = ColumnDirection::DOWN;
    LayoutArrangement arrangement = LayoutArrangement::START;
    LayoutAlignment alignment = LayoutAlignment::START;

    Modifier modifier{};

    std::vector<UIComponentFactory> children;
};

class UIColumnComponent : public UIComponent<UIColumnComponent, UIColumnProps> {
public:
    UIColumnComponent (UI& ui, UIColumnProps props);

    UIRenderResult render (UIContext& ctx) const override;

private:
    LayoutAxis axis () const noexcept;
};

struct UIRowProps {
    RowDirection direction = RowDirection::RIGHT;
    LayoutArrangement arrangement = LayoutArrangement::START;
    LayoutAlignment alignment = LayoutAlignment::START;

    Modifier modifier{};

    std::vector<UIComponentFactory> children;
};

class UIRowComponent : public UIComponent<UIRowComponent, UIRowProps> {
public:
    UIRowComponent (UI& ui, UIRowProps props);

    UIRenderResult render (UIContext& ctx) const override;

private:
    LayoutAxis axis () const noexcept;
};
} // namespace phenyl::graphics
