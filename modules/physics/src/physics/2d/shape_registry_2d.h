#pragma once

#include "component/fixed_component.h"
#include "shapes/box_shape_2d_new.h"

namespace physics {
    using ShapeRegistry2D = component::FixedComponentManager<8, BoxShape2DNew>;
}