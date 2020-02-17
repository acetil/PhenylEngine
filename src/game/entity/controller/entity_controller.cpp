#include "entity_controller.h"
#include "logging/logging.h"
using namespace game;

void game::EntityController::controlEntityPrePhysics (AbstractEntity* entity, component::EntityMainComponent* comp,
                                                      int entityId, component::ComponentManager* manager) {
        // default is empty
}
void game::EntityController::controlEntityPostPhysics (AbstractEntity* entity, component::EntityMainComponent* comp,
                                                       int entityId, component::ComponentManager* manager) {
    // default is empty
}
void game::EntityController::onEntityCollision (AbstractEntity* entity, int entityId, AbstractEntity* other, int otherEntityId,
                                                component::ComponentManager* manager) {
    // default is empty
}
void game::controlEntitiesPrePhysics (AbstractEntity** entities, component::EntityMainComponent* comp, int startId, int numEntities,
                                      int direction, component::ComponentManager* manager) {
    for (int i = 0; i < numEntities; i++) {
        entities[i]->getController()->controlEntityPrePhysics(entities[i * direction], comp + i * direction,
                startId + i * direction, manager);
    }
}
void game::controlEntitiesPostPhysics(AbstractEntity** entities, component::EntityMainComponent* comp, int startId, int numEntities,
                                      int direction, component::ComponentManager* manager) {
    for (int i = 0; i < numEntities; i++) {
        entities[i]->getController()->controlEntityPostPhysics(entities [i * direction], comp + i * direction,
                startId + i * direction, manager);
    }
}

