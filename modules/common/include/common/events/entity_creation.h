#pragma once

#include <utility>

#include "event/event.h"
#include "component/component.h"
#include "component/view/game_view.h"
#ifndef ENTITY_H
namespace game {
class AbstractEntity;
};
#endif

namespace event {
    class EntityCreationEvent : public Event<EntityCreationEvent> {
    public:
        inline static const std::string name = "entity_creation";
        //float x;
        //float y;
        //float size;
        component::EntityComponentManager& compManager;
        //game::AbstractEntity* entity{};
        //component::EntityId entityId;
        component::Entity entityView;
        view::GameView gameView;
        /*EntityCreationEvent () : gameView(nullptr) {
            x = 0;
            y = 0;
            size = 0;
            compManager = nullptr;
            entityId = 0;
        };*/
        EntityCreationEvent (component::EntityComponentManager& compManager, component::Entity _entityView, view::GameView _gameView) :
                entityView{_entityView}, gameView{_gameView}, compManager{compManager} {}
    };
}
