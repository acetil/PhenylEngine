#include "entity_controller.h"

using namespace game;

void game::EntityController::controlEntityPrePhysics (AbstractEntity *entity, component::EntityMainComponent *comp,
                                                      int entityId, component::ComponentManager *manager) {
        // default is empty
}
void game::EntityController::controlEntityPostPhysics (AbstractEntity *entity, component::EntityMainComponent *comp,
                                                       int entityId, component::ComponentManager *manager) {
    // default is empty
}
void game::EntityController::onEntityCollision (AbstractEntity *entity, int entityId, AbstractEntity *other, int otherEntityId,
                                                component::ComponentManager *manager) {
    // default is empty
}