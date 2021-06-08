#include "component/view/game_view.h"
#include "component/component.h"
#include "component/main_component.h"
#include "event/events/entity_creation.h"
#include "event/events/entity_rotation.h"

#ifndef PHYSICS_H
#define PHYSICS_H
namespace physics {
    void onEntityCreation (event::EntityCreationEvent& event);
    void updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager);
    void checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView gameView);
    void updateEntityHitboxRotation (event::EntityRotationEvent& event);
}
#endif