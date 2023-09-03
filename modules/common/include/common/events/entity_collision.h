#pragma once

#include "component/component.h"
#include "component/view/game_view.h"
#include "event/event.h"
#include "event/event_bus.h"
#include "component/view/game_view.h"
#include <string>
#include <utility>

namespace event {
    class EntityCollisionEvent : public Event<EntityCollisionEvent, false> {
    public:
        //component::EntityId entityId;
        component::Entity firstEntity;
        //component::EntityId otherId;
        component::Entity secondEntity;
        unsigned int collisionLayers;
        inline static const std::string name = "entity_collision";
        component::EntityComponentManager& componentManager;
        EventBus::SharedPtr eventBus; // TODO: find better way
        view::GameView& gameView;
        //EntityCollisionEvent () : entityId(0), otherId(0), collisionLayers(0), componentManager(), eventBus{}, gameView(view::GameView(nullptr)) {}
        EntityCollisionEvent (component::Entity firstEntity, component::Entity secondEntity, unsigned int _collisionLayers,
                              component::EntityComponentManager& compManager, EventBus::SharedPtr bus, view::GameView& _gameView) : firstEntity(std::move(firstEntity)),
                                    secondEntity(std::move(secondEntity)), collisionLayers(_collisionLayers), componentManager(compManager), eventBus(std::move(bus)), gameView(_gameView) {};
    };
}
