#include "entity_type.h"
#include "component/component.h"
#ifndef ENTITY_TYPE_FUNCTIONS_H
#define ENTITY_TYPE_FUNCTIONS_H
namespace game {
    void setInitialEntityValues (const component::EntityComponentManager::SharedPtr& componentManager,
                                 EntityType& type, int entityId, float x, float y, float rot);
}
#endif //ENTITY_TYPE_FUNCTIONS_H
