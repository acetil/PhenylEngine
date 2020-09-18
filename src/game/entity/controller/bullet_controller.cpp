#include "bullet_controller.h"

void game::BulletController::onEntityCollision (view::EntityView &entityView, view::GameView& gameView, int otherEntityId, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.entityId);
}
