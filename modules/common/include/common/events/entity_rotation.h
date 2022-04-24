#pragma once

#include <utility>

#include "component/view/view.h"
#include "component/component_new.h"
#include "event/event.h"

namespace event {
    class EntityRotationEvent : public Event<EntityRotationEvent, false> {
    public:
        inline static const std::string name = "entity_rotation";
        glm::mat2 rotMatrix;
        glm::mat2 prevMatrix;
        component::EntityId entityId;
        component::EntityComponentManager::SharedPtr manager;
        //EntityRotationEvent () : rotMatrix(), prevMatrix(), entityId(0), manager{} {};
        EntityRotationEvent (component::EntityId _entityId, component::EntityComponentManager::SharedPtr _manager, glm::mat2 _prevMatrix, glm::mat2 _rotMatrix) :
            entityId(_entityId), manager(std::move(_manager)), prevMatrix(_prevMatrix), rotMatrix(_rotMatrix) {};
    };
}
