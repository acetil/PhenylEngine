#pragma once
#include "ui/reactive/component.h"
#include "ui/reactive/ui.h"
#include "ui/widgets/layout.h"

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

    std::function<void(UI&)> children = [] (UI&) {
    };
};

class UILayoutComponent : public UIComponent<UILayoutComponent, UILayoutProps> {
public:
    UILayoutComponent (UI& ui, UILayoutProps props);

    void render (UI& ui) const override;
};

struct UIColumnProps {
    ColumnDirection direction = ColumnDirection::DOWN;
    LayoutArrangement arrangement = LayoutArrangement::START;
    LayoutAlignment alignment = LayoutAlignment::START;

    Modifier modifier{};

    std::function<void(UI&)> children = [] (UI&) {
    };
};

class UIColumnComponent : public UIComponent<UIColumnComponent, UIColumnProps> {
public:
    UIColumnComponent (UI& ui, UIColumnProps props);

    void render (UI& ui) const override;

private:
    LayoutAxis axis () const noexcept;
};

struct UIRowProps {
    RowDirection direction = RowDirection::RIGHT;
    LayoutArrangement arrangement = LayoutArrangement::START;
    LayoutAlignment alignment = LayoutAlignment::START;

    Modifier modifier{};

    std::function<void(UI&)> children = [] (UI&) {
    };
};

class UIRowComponent : public UIComponent<UIRowComponent, UIRowProps> {
public:
    UIRowComponent (UI& ui, UIRowProps props);

    void render (UI& ui) const override;

private:
    LayoutAxis axis () const noexcept;
};
} // namespace phenyl::graphics
