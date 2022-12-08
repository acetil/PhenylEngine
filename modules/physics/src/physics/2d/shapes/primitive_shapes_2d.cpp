#include "primitive_shapes_2d.h"

using namespace physics;

physics::BoxShape2D::BoxShape2D (physics::ShapeId id, physics::ColliderId collider, physics::ShapeId parent) : CollisionShape2D(id, collider, parent, true, true) {}

util::Optional<CollisionResponse2D> physics::BoxShape2D::collisionDetectionAccept (physics::CollisionShape2D& other, glm::vec2 displacement) {
    return other.collisionDetectionVisit(*this, displacement);
}

util::Optional<CollisionResponse2D> BoxShape2D::collisionDetectionVisit (BoxShape2D& other, glm::vec2 displacement) {
    return util::Optional<CollisionResponse2D>();
}

util::Optional<CollisionResponse2D> BoxShape2D::collisionDetectionVisit (CircleShape2D& other, glm::vec2 displacement) {
    return util::Optional<CollisionResponse2D>();
}


CircleShape2D::CircleShape2D (float radius, ShapeId id, ColliderId collider, ShapeId parent)
        : CollisionShape2D(id, collider, parent, false, false), localRadius{radius}, worldRadius{radius} {
    setTransform({{radius, 0}, {0, radius}});
}

util::Optional<CollisionResponse2D> CircleShape2D::collisionDetectionAccept (CollisionShape2D& other, glm::vec2 displacement) {
    return util::Optional<CollisionResponse2D>();
}

util::Optional<CollisionResponse2D> CircleShape2D::collisionDetectionVisit (BoxShape2D& other, glm::vec2 displacement) {
    return util::Optional<CollisionResponse2D>();
}

util::Optional<CollisionResponse2D> CircleShape2D::collisionDetectionVisit (CircleShape2D& other, glm::vec2 displacement) {
    return util::Optional<CollisionResponse2D>();
}

void CircleShape2D::setRadius (float radius) {
    setTransform({{radius, 0}, {0, radius}});
}
