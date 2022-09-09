#include "engine/entity/controller/bullet_controller.h"

#include "util/string_help.h"

void game::BulletController::onEntityCollision (component::view::EntityView &entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.getId());
}

int game::BulletController::getTextureId (component::view::EntityView& entityView, view::GameView& gameView) const {
    return texId;
}

void game::BulletController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test9");
}

void game::BulletController::initEntity (component::view::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    const auto& velocity = data.get<util::DataObject>().at("velocity").get<util::DataObject>();
    entityView.getComponent<component::FrictionKinematicsMotion2D>().ifPresent([&velocity] (component::FrictionKinematicsMotion2D& comp) {
       comp.velocity = glm::vec2{velocity.at("x"), velocity.at("y")};
    });
    //entityView.velocity = glm::vec2{velocity.at("x"), velocity.at("y")};
}
