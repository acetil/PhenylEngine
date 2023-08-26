#include "physics/phenyl_physics.h"
#include "physics/physics.h"

using namespace physics;

PhenylPhysics::PhenylPhysics (physics::IPhysics* physicsObj) : physicsObj{physicsObj} {}

void PhenylPhysics::updatePhysics (component::EntityComponentManager& componentManager, float deltaTime) {
    physicsObj->updatePhysics(componentManager, deltaTime);
}

void PhenylPhysics::checkCollisions (component::EntityComponentManager& componentManager,
                                     const std::shared_ptr<event::EventBus>& eventBus, view::GameView& gameView) {
    physicsObj->checkCollisions(componentManager, eventBus, gameView);
}

void PhenylPhysics::debugRender (const component::EntityComponentManager& componentManager) {
    physicsObj->debugRender(componentManager);
}

