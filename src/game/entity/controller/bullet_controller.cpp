#include "engine/entity/controller/bullet_controller.h"

#include "util/string_help.h"

void game::BulletController::onEntityCollision (view::EntityView &entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.entityId);
}

int game::BulletController::getTextureId (view::EntityView& entityView, view::GameView& gameView) {
    return texId;
}

void game::BulletController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test9");
}

void game::BulletController::initEntity (view::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    const util::DataObject& velocity = data.get<util::DataObject>().at("vel").get<util::DataObject>();
    entityView.velocity = glm::vec2{velocity.at("x"), velocity.at("y")};
}
