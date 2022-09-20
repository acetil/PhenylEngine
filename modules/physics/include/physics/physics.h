#pragma once

#include "component/view/game_view.h"
#include "component/component.h"
#include "component/forward.h"
#include "physics/components/2D/simple_friction.h"
#include "common/events/entity_creation.h"
#include "common/events/entity_rotation.h"
#include "event/event_bus.h"

namespace physics {
    void onEntityCreation (event::EntityCreationEvent& event);
    void updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager);
    void checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView gameView);
    void addComponentSerialisers (component::EntitySerialiser& serialiser);
}
