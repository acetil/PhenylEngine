#include "engine/entity/controller/bullet_controller.h"
#include "physics/components/2D/rigid_body.h"

void game::BulletController::onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.id());
}

void game::BulletController::initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {

}

game::BulletController::BulletController () : EntityController("bullet") {}
