#include "component/component.h"
#include "component/main_component.h"
#include "event/events/entity_creation.h"
#ifndef PHYSICS_NEW_H
#define PHYSICS_HEW_H
namespace physics {
    void onEntityCreation (event::EntityCreationEvent& event);
    void updatePhysics (component::ComponentManager<game::AbstractEntity*>* componentManager);
    void checkCollisions (component::ComponentManager<game::AbstractEntity*>* componentManager);
}
#endif