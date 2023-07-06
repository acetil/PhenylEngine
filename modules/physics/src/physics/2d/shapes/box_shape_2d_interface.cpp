#include "physics/shape/2d/box_shape_2d_interface.h"
#include "box_shape_2d_new.h"

using namespace physics;

glm::vec2 ShapeView<BoxShape2DNew>::getScale () {
    return shape.getScale();
}

void ShapeView<BoxShape2DNew>::setScale (glm::vec2 scale) {
    shape.setScale(scale);
}


std::uint64_t ShapeView<BoxShape2DNew>::getLayers () {
    return shape.getLayers();
}

void ShapeView<BoxShape2DNew>::setLayers (std::uint64_t layers) {
    shape.setLayers(layers);
}

std::uint64_t ShapeView<BoxShape2DNew>::getMask () {
    return shape.getMask();
}

void ShapeView<BoxShape2DNew>::setMask (std::uint64_t mask) {
    shape.setMask(mask);
}

BoxShape2DNew ShapeRequest<BoxShape2DNew>::make (physics::ColliderId collider) {
    return BoxShape2DNew{collider, scale, layers, mask};
}