#include "box_shape_2d.h"

using namespace physics;

static float calculateRadius (glm::mat2 transform) {
    glm::vec2 xVec = transform[0];
    glm::vec2 yVec = transform[1];

    auto cornerVec = xVec + yVec;

    return glm::length(cornerVec);
}

BoxShape2D::BoxShape2D (glm::mat2 localTransform, std::uint64_t layers, std::uint64_t mask) : Shape2D(calculateRadius(localTransform), layers, mask), localTransform{localTransform}, transform{localTransform} {

}

void BoxShape2D::applyWorldTransform (glm::mat2 worldTransform) {
    transform = localTransform * worldTransform;
    outerRadius = calculateRadius(transform);
}

void BoxShape2D::setLocalTransform (glm::mat2 _localTransform) {
    auto worldTransform = glm::inverse(localTransform) * transform;
    localTransform = _localTransform;
    transform = localTransform * worldTransform;
    outerRadius = calculateRadius(transform);
}

util::Optional<CollisionResponse2D> physics::collideShape2D (const BoxShape2D& shape1, const ShapeVariant2D& shape2, glm::vec2 disp) {
    return std::visit([shape1, disp] (const auto& shape) {
        return collideShape2D(shape1, shape, disp);
    }, shape2);
}

util::Optional<CollisionResponse2D> physics::collideShape2D (const BoxShape2D& shape1, const BoxShape2D& shape2, glm::vec2 disp) {
    auto inv1 = glm::inverse(shape1.transform);
    auto inv2 = glm::inverse(shape2.transform);

    glm::vec2 basisVecs[2] = {{1, 0}, {0, 1}};
    float leastSquaredDist = FLT_MAX;
    glm::vec2 minDispVec;

    for (int i = 0; i < 4; i++) {
        glm::vec2 basis1 = basisVecs[i % 2];
        bool isIntersection = false;
        glm::vec2 minIntersectionVec = {FLT_MAX, FLT_MAX};
        glm::vec2 maxIntersectionVec = {FLT_MIN, FLT_MIN};
    }
}