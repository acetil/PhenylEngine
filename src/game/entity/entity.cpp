#include "entity.h"
#include "physics/physics.h"
#include "physics/physics_body.h"
#include "logging/logging.h"
using namespace game;

void game::AbstractEntity::setEntityId (int entityId) {
    this->entityId = entityId;
}
int game::AbstractEntity::getEntityId () {
    return entityId;
}
std::string game::AbstractEntity::getEntityName () {
    return name;
}
void game::AbstractEntity::update (float time) {
    // default is empty
}
// PhysicsBody delegates
void game::AbstractEntity::addForce (physics::Force force) {
    physicsBody->addForce(force);
}
void game::AbstractEntity::clearMovementForces (int tag) {
    physicsBody->removeMovementForces(tag);
}
void game::AbstractEntity::setVelocity (float velX, float velY) {
    physicsBody->setVelocity(velX, velY);
}
void game::AbstractEntity::updatePosition (float addedTime) {
    physicsBody->updatePosition(addedTime);
}
int game::AbstractEntity::getTextureId () {
    return textureId;
}
// graphics code for base class

/*void game::AbstractEntity::render (graphics::Graphics* graphics) {
    graphics->drawTexSquare(*x, *y, scale, textureId);
}*/
EntityController* game::AbstractEntity::getController() {
    return controller;
}