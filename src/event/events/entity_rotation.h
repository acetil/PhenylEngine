#include "component/view/view.h"
#include "event/event.h"
#ifndef ENTITY_ROTATION_H
#define ENTITY_ROTATION_H
namespace event {
    class EntityRotationEvent : public Event<EntityRotationEvent, false> {
    public:
        std::string name = "entity_rotation";
        glm::mat2 rotMatrix;
        glm::mat2 prevMatrix;
        int entityId;
        component::EntityComponentManager* manager;
        EntityRotationEvent () : rotMatrix(), prevMatrix(), entityId(0), manager(nullptr) {};
        EntityRotationEvent (int _entityId, component::EntityComponentManager* _manager, glm::mat2 _prevMatrix, glm::mat2 _rotMatrix) :
            entityId(_entityId), manager(_manager), prevMatrix(_prevMatrix), rotMatrix(_rotMatrix) {};
    };
}
#endif
