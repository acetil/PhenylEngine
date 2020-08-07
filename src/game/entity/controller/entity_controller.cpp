#include "entity_controller.h"
#include "logging/logging.h"
using namespace game;

void game::EntityController::controlEntityPrePhysics (AbstractEntity* entity, component::EntityMainComponent* comp,
                                                      int entityId, component::ComponentManager<AbstractEntity*>* manager) {
    logging::log(LEVEL_INFO, "Controlling entity pre physics!");
    // default is empty
}
void game::EntityController::controlEntityPostPhysics (AbstractEntity* entity, component::EntityMainComponent* comp,
                                                       int entityId, component::ComponentManager<AbstractEntity*>* manager) {
    // default is empty
}
void game::EntityController::onEntityCollision (AbstractEntity* entity, int entityId, AbstractEntity* other, int otherEntityId,
                                                component::ComponentManager<AbstractEntity*>* manager, unsigned int layers) {
    logging::log(LEVEL_DEBUG, "On entity collision!");
}
void game::controlEntitiesPrePhysics (AbstractEntity** entities, component::EntityMainComponent* comp, int startId, int numEntities,
                                      int direction, component::ComponentManager<AbstractEntity*>* manager) {
    for (int i = 0; i < numEntities; i++) {
        entities[i]->getController()->controlEntityPrePhysics(entities[i * direction], comp + i * direction,
                startId + i * direction, manager);
    }
}
void game::controlEntitiesPostPhysics(AbstractEntity** entities, component::EntityMainComponent* comp, int startId, int numEntities,
                                      int direction, component::ComponentManager<AbstractEntity*>* manager) {
    for (int i = 0; i < numEntities; i++) {
        entities[i]->getController()->controlEntityPostPhysics(entities [i * direction], comp + i * direction,
                startId + i * direction, manager);
    }
}

void game::controlOnCollision (event::EntityCollisionEvent& collisionEvent) {
    auto control = collisionEvent.componentManager->getObjectData<AbstractEntity*>(0, collisionEvent.entityId)->getController();
    control->onEntityCollision(collisionEvent.componentManager->getObjectData<AbstractEntity*>(0, collisionEvent.entityId),
            collisionEvent.entityId,collisionEvent.componentManager->getObjectData<AbstractEntity*>(0, collisionEvent.otherId),
            collisionEvent.otherId, collisionEvent.componentManager, collisionEvent.collisionLayers);
}

void game::addControlEventHandlers (event::EventBus* eventBus) {
    eventBus->subscribeHandler(controlOnCollision);
}