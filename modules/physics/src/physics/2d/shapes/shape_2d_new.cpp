#include "shape_2d_new.h"

using namespace physics;

Shape2DNew::Shape2DNew (ColliderId collider, float outerRadius, std::uint64_t layers, std::uint64_t mask) : collider{collider}, outerRadius{outerRadius}, layers{layers}, mask{mask} {}

std::uint64_t Shape2DNew::getLayers () const {
    return layers;
}

std::uint64_t Shape2DNew::getMask () const {
    return mask;
}

void Shape2DNew::setLayers (std::uint64_t newLayers) {
    layers = newLayers;
}

void Shape2DNew::setMask (std::uint64_t newMask) {
    mask = newMask;
}

void Shape2DNew::setOuterRadius (float newOuterRadius) {
    outerRadius = newOuterRadius;
}

ColliderId Shape2DNew::getColliderId () const {
    return collider;
}

bool Shape2DNew::shouldCollide (const Shape2DNew& other) const {
    auto displacement = getDisplacement(other);
    float sqDispLen = displacement.x * displacement.x + displacement.y * displacement.y;
    float radiusLen = outerRadius + other.outerRadius;

    return (layers & other.mask || other.layers & mask) && sqDispLen < radiusLen * radiusLen;
}

void Shape2DNew::setPosition (glm::vec2 _position) {
    position = _position;
}

glm::vec2 Shape2DNew::getDisplacement (const Shape2DNew& other) const {
    return other.position - position;
}
