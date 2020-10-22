#include "event/event.h"
#include "component/component.h"
#include "component/view/view.h"
#include "component/view/game_view.h"
#ifndef ENTITY_H
namespace game {
class AbstractEntity;
};
#endif
#ifndef ENTITY_CREATION_H
#define ENTITY_CREATION_H
namespace event {
    class EntityCreationEvent : public Event<EntityCreationEvent> {
    public:
        std::string name = "entity_creation";
        float x;
        float y;
        float size;
        component::EntityComponentManager* compManager;
        game::AbstractEntity* entity{};
        int entityId;
        view::EntityView entityView;
        view::GameView gameView;
        event::EventBus* eventBus = nullptr; // TODO: remove
        EntityCreationEvent () : gameView(nullptr) {
            x = 0;
            y = 0;
            size = 0;
            compManager = nullptr;
            entityId = 0;
        };
        EntityCreationEvent (float x, float y, float size, component::EntityComponentManager* compManager, game::AbstractEntity* entity, int entityId,
                             view::EntityView _entityView, view::GameView _gameView) : entityView(std::move(_entityView)), gameView(_gameView) {
            this->x = x;
            this->y = y;
            this->size = size;
            this->compManager = compManager;
            this->entity = entity;
            this->entityId = entityId;
        };
    };
}
#endif