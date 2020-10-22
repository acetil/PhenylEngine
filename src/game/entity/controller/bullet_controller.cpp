#include "bullet_controller.h"

void game::BulletController::onEntityCollision (view::EntityView &entityView, view::GameView& gameView, int otherEntityId, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.entityId);
}

int game::BulletController::getTextureId (view::EntityView& entityView, view::GameView& gameView) {
    return texId;
}

void game::BulletController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test9");
}
