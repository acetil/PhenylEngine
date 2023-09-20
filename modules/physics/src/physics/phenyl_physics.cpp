#include "physics/phenyl_physics.h"
#include "physics/physics.h"

using namespace phenyl::physics;

PhenylPhysics::PhenylPhysics (physics::IPhysics* physicsObj) : physicsObj{physicsObj} {}

void PhenylPhysics::updatePhysics (component::EntityComponentManager& componentManager, float deltaTime) {
    physicsObj->updatePhysics(componentManager, deltaTime);
}

void PhenylPhysics::checkCollisions (component::EntityComponentManager& componentManager, float deltaTime) {
    physicsObj->checkCollisions(componentManager, deltaTime);
}

void PhenylPhysics::debugRender (const component::EntityComponentManager& componentManager) {
    physicsObj->debugRender(componentManager);
}

