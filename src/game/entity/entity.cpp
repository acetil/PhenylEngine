#include "entity.h"
#include "physics/physics.h"
#include "physics/physics_body.h"
#include "graphics/graphics.h"

using namespace game;

void game::AbstractEntity::setEntityId (int entityId) {
    this->entityId = entityId;
}
int game::AbstractEntity::getEntityId () {
    return entityId;
}

// PhysicsBody delegates
void game::AbstractEntity::addForce (physics::Force force) {
    physicsBody->addForce(force);
}
void game::AbstractEntity::clearMovementForces () {
    physicsBody->removeMovementForces();
}
void game::AbstractEntity::setVelocity (float velX, float velY) {
    physicsBody->setVelocity(velX, velY);
}
void game::AbstractEntity::updatePosition (float addedTime) {
    physicsBody->updatePosition(addedTime);
}
// graphics code for base class

void game::AbstractEntity::render (graphics::Graphics* graphics) {
    graphics->drawTexSquare(x, y, scale, textureId);
}