#include <math.h>
#include <stdlib.h>

#include "component.h"
#include "logging/logging.h"
#include "event/events/entity_id_swap.h"
#include "game/entity/entity.h"
using namespace component;

component::ComponentManager::ComponentManager(int maxEntities, event::EventBus* bus) {
    this->maxEntities = fmin(maxEntities, MAX_COMPONENT_ENTITIES);
    this->numEntities = 0;
    this->bus = bus;
    addComponent<game::AbstractEntity*>("entity_ptr"); // TODO: when AbstractEntity is not abstract, remove pointer
}
component::ComponentManager::~ComponentManager () {
    for (Component comp : components) {
        delete comp.data;
    }
}

id_type_t component::ComponentManager::getNumEntities () {
    return numEntities;
}
id_type_t component::ComponentManager::addEntity (game::AbstractEntity* entity) {
    logging::logf(LEVEL_DEBUG, "Num entities: %d", numEntities);
    getComponent<game::AbstractEntity*>(0)[numEntities] = entity;
    return numEntities++;
}
void component::ComponentManager::removeEntity (id_type_t entityId) {
    if (entityId >= numEntities) {
        logging::logf(LEVEL_ERROR, "Attempted to remove entity id %d, only %d entity components!", entityId, numEntities);
        return;
    }
    id_type_t oldId = --numEntities;
    for (Component c : components) {
        memcpy(c.data + entityId * c.size, c.data + oldId * c.size, c.size);
    }
    bus->raiseEvent(new event::EntityIdSwapEvent(oldId, entityId));
}