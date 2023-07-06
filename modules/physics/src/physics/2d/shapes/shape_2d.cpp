#include "shape_2d.h"

physics::CollisionShape2DBase::CollisionShape2DBase (physics::ShapeId id, physics::ColliderId collider, physics::ShapeId parent, bool canRotate, bool canDistort)
    : id{id},
      collider{collider},
      parent{parent},
      canRotate{canRotate},
      canDistort{canDistort},
      localTransform{{1, 0}, {0, 1}},
      localDisp{0, 0},
      worldTransform{{1, 0}, {0, 1}},
      worldDisp{0, 0},
      dirty{true} {}

physics::ShapeId physics::CollisionShape2DBase::getId () {
    return id;
}

physics::ColliderId physics::CollisionShape2DBase::getCollider () {
    return collider;
}

physics::ShapeId physics::CollisionShape2DBase::getParent () {
    return parent;
}

bool physics::CollisionShape2DBase::isDirty () {
    return dirty;
}

void physics::CollisionShape2DBase::markDirty () {
    dirty = true; // TODO
}

void physics::CollisionShape2DBase::setTransform (glm::mat2 transform) {
    if (!canRotate || !canDistort) {
        glm::mat2 scaleMat = {{glm::length(transform[0]), 0}, {0, glm::length(transform[1])}};

        if (!canRotate && transform != scaleMat) {
            logging::log(LEVEL_DEBUG, "Attempted to apply rotated transform to unrotatable collision shape!");
            transform = scaleMat;
        }

        if (!canDistort && glm::abs(scaleMat[0][0] - scaleMat[1][1]) < FLT_EPSILON) {
            logging::log(LEVEL_ERROR, "Attempted to apply stretched transform to undistortable collision shape!");
            float transformFactor = glm::sqrt(scaleMat[0][0] * scaleMat[0][0] + scaleMat[1][1] * scaleMat[1][1]);
            transform = {{transformFactor, 0}, {0, transformFactor}};
        }
    }

    markDirty();
}

void physics::CollisionShape2DBase::setDisplacement (glm::vec2 localDisplacement) {
    localDisp = localDisplacement;
    markDirty();
}

void physics::CollisionShape2DBase::updateTransforms (glm::mat2 worldTransform, glm::vec2 worldDisplacement) {
    this->worldTransform = localTransform * worldTransform;
    this->worldDisp = worldTransform * localDisp + worldDisplacement;

    // TODO
    auto p = updateGeometry();
    worldRelLowBound = p.first;
    worldRelHighBound = p.second;
}


physics::CollisionShape2DBase::~CollisionShape2DBase () noexcept = default;

physics::CollisionShape2D::CollisionShape2D (physics::ShapeId id, physics::ColliderId collider, physics::ShapeId parent, bool canRotate, bool canDistort)
        : CollisionShape2DBase(id, collider, parent, canRotate, canDistort) {}

bool physics::Shape2D::shouldCollide (const Shape2D& other, glm::vec2 displacement) const {
    float radiusSum = outerRadius + other.outerRadius;
    return (layers & other.mask || other.layers & mask) && (radiusSum * radiusSum) < (displacement.x * displacement.x + displacement.y * displacement.y);
}