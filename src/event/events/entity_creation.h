#include "event/event.h"
#include "component/component.h"
#include "graphics/graphics.h"
#ifndef ENTITY_H
namespace game {
class AbstractEntity;
};
#endif
#ifndef ENTITY_CREATION_H
#define ENTITY_CREATION_H
namespace event {
    static EventType* ENTITY_CREATION_EVENT = nullptr;
    class EntityCreationEvent : public Event {
        public:
        float x;
        float y;
        float size;
        component::ComponentManager* compManager;
        graphics::Graphics* graphics;
        game::AbstractEntity* entity;
        int entityId;
        EntityCreationEvent () {
            x = 0;
            y = 0;
            size = 0;
            compManager = nullptr;
            graphics = nullptr;
            entityId = 0;
        };
        EntityCreationEvent (float x, float y, float size, component::ComponentManager* compManager, graphics::Graphics* graphics, game::AbstractEntity* entity, int entityId) {
            this->x = x;
            this->y = y;
            this->size = size;
            this->compManager = compManager;
            this->graphics = graphics;
            this->entity = entity;
            this->entityId = entityId;
        };
        virtual EventType* getEventType () {
            if (ENTITY_CREATION_EVENT == nullptr) {
                ENTITY_CREATION_EVENT = new EventType("entity_creation", false, true);
            }
            return ENTITY_CREATION_EVENT;
        }
    };
}
#endif