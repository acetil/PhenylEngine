#pragma once

#include "engine/entity/entity_type.h"
#include "component/component.h"

namespace game {
    void setInitialEntityValues (const component::EntityComponentManager::SharedPtr& componentManager,
                                 EntityType& type, int entityId, float x, float y, float rot);
}
