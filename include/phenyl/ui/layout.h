#pragma once

#include "ui/reactive/components/layout.h"
#include "ui/widgets/layout.h"

namespace phenyl::ui {
using LayoutWidget = phenyl::graphics::LayoutWidget;
using ColumnWidget = phenyl::graphics::ColumnWidget;
using RowWidget = phenyl::graphics::RowWidget;

using LayoutArrangement = phenyl::graphics::LayoutArrangement;
using LayoutAlignment = phenyl::graphics::LayoutAlignment;

using ColumnDirection = phenyl::graphics::ColumnDirection;
using RowDirection = phenyl::graphics::RowDirection;

using LayoutAxis = phenyl::graphics::LayoutAxis;

using LayoutProps = graphics::UILayoutProps;
using Layout = phenyl::graphics::UILayoutComponent;

using ColumnProps = graphics::UIColumnProps;
using Column = phenyl::graphics::UIColumnComponent;

using RowProps = graphics::UIRowProps;
using Row = phenyl::graphics::UIRowComponent;
} // namespace phenyl::ui
