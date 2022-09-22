#pragma once

#include "component/view/game_view.h"
#include "component/component.h"
#include "component/forward.h"
#include "physics/components/simple_friction.h"
#include "common/events/entity_creation.h"
#include "common/events/entity_rotation.h"
#include "event/event_bus.h"

namespace physics {
    class IPhysics {
    public:
        virtual ~IPhysics() = default;
        virtual void addComponentSerialisers (component::EntitySerialiser& serialiser) = 0;
        virtual void updatePhysics (const component::EntityComponentManager::SharedPtr& componentManager) = 0;
        virtual void checkCollisions (const component::EntityComponentManager::SharedPtr& componentManager, const event::EventBus::SharedPtr& eventBus, view::GameView& gameView) = 0;
    };

    std::unique_ptr<IPhysics> makeDefaultPhysics ();
}
