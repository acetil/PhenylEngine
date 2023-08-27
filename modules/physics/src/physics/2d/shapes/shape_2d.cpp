#include "shape_2d.h"

using namespace physics;

Shape2D::Shape2D (ColliderId collider, float outerRadius, std::uint64_t layers, std::uint64_t mask) : collider{collider}, outerRadius{outerRadius}, layers{layers}, mask{mask} {}

std::uint64_t Shape2D::getLayers () const {
    return layers;
}

std::uint64_t Shape2D::getMask () const {
    return mask;
}

void Shape2D::setLayers (std::uint64_t newLayers) {
    layers = newLayers;
}

void Shape2D::setMask (std::uint64_t newMask) {
    mask = newMask;
}

void Shape2D::setOuterRadius (float newOuterRadius) {
    outerRadius = newOuterRadius;
}

ColliderId Shape2D::getColliderId () const {
    return collider;
}

bool Shape2D::shouldCollide (const Shape2D& other) const {
    auto displacement = getDisplacement(other);
    float sqDispLen = displacement.x * displacement.x + displacement.y * displacement.y;
    float radiusLen = outerRadius + other.outerRadius;

    return (layers & other.mask || other.layers & mask) && sqDispLen < radiusLen * radiusLen;
}

void Shape2D::setPosition (glm::vec2 _position) {
    position = _position;
}

glm::vec2 Shape2D::getDisplacement (const Shape2D& other) const {
    return other.position - position;
}

glm::vec2 Shape2D::getPosition () {
    return position;
}
