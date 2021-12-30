#include <utility>

#include "component/view/view.h"
#include "event/event.h"
#ifndef ENTITY_ROTATION_H
#define ENTITY_ROTATION_H
namespace event {
    class EntityRotationEvent : public Event<EntityRotationEvent, false> {
    public:
        inline static const std::string name = "entity_rotation";
        glm::mat2 rotMatrix;
        glm::mat2 prevMatrix;
        int entityId;
        component::EntityComponentManager::SharedPtr manager;
        EntityRotationEvent () : rotMatrix(), prevMatrix(), entityId(0), manager{} {};
        EntityRotationEvent (int _entityId, component::EntityComponentManager::SharedPtr _manager, glm::mat2 _prevMatrix, glm::mat2 _rotMatrix) :
            entityId(_entityId), manager(std::move(_manager)), prevMatrix(_prevMatrix), rotMatrix(_rotMatrix) {};
    };
}
#endif
