#include "bullet_controller.h"

void game::BulletController::onEntityCollision (view::EntityView &entityView, int otherEntityId, unsigned int layers) {
    EntityController::onEntityCollision(entityView, otherEntityId, layers);
}
