#include "component/component.h"
#include "component/main_component.h"
#include "event/events/entity_creation.h"
#ifndef PHYSICS_H
#define PHYSICS_H
namespace physics {
    void onEntityCreation (event::EntityCreationEvent& event);
    void updatePhysics (component::EntityComponentManager* componentManager);
    void checkCollisions (component::EntityComponentManager* componentManager, event::EventBus* eventBus);
}
#endif