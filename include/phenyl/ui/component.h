#pragma once

#include "ui/reactive/component.h"

namespace phenyl::ui {
template <typename CompType, typename Props>
using Component = graphics::UIComponent<CompType, Props>;
}
