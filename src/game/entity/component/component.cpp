#include <math.h>
#include <stdlib.h>

#include "component.h"
#include "logging/logging.h"
#include "event/events.h"
using namespace game;

game::ComponentManager::ComponentManager(id_type_t maxEntities, event::EventBus* bus) {
    this->maxEntities = fmin(maxEntities, MAX_COMPONENT_ENTITIES);
    this->numEntities = 0;
    this->bus = bus;
    addComponent<AbstractEntity*>("entity_ptr");
}
game::ComponentManager::~ComponentManager () {
    for (Component comp : components) {
        delete comp.data;
    }
}

id_type_t game::ComponentManager::getNumEntities () {
    return numEntities;
}
id_type_t game::ComponentManager::addEntity (AbstractEntity* entity) {
    entity->setEntityId(numEntities);
    return numEntities++;
}
void game::ComponentManager::removeEntity (id_type_t entityId) {
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