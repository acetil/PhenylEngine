#include "event/event.h"
#include "component/component.h"
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
        component::ComponentManager<game::AbstractEntity*>* compManager;
        game::AbstractEntity* entity{};
        int entityId;
        EntityCreationEvent () {
            x = 0;
            y = 0;
            size = 0;
            compManager = nullptr;
            entityId = 0;
        };
        EntityCreationEvent (float x, float y, float size, component::ComponentManager<game::AbstractEntity*>* compManager, game::AbstractEntity* entity, int entityId) {
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