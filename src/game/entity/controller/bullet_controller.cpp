#include "engine/entity/controller/bullet_controller.h"
#include "graphics/textures/texture_atlas.h"
#include "physics/components/simple_friction.h"
#include "physics/components/2D/rigid_body.h"

#include "util/string_help.h"

void game::BulletController::onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.id());
}

/*int game::BulletController::getTextureId (component::EntityView& entityView, view::GameView& gameView) const {
    return texId;
}

void game::BulletController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test9");
}*/

void game::BulletController::initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    /*const auto& momentum = data.get<util::DataObject>().at("momentum").get<util::DataObject>();
    entityView.get<physics::RigidBody2D>().ifPresent([&momentum] (physics::RigidBody2D& comp) {
       //comp.momentum = glm::vec2{momentum.at("x"), momentum.at("y")};
       comp.applyImpulse(momentum)
    });*/
    //entityView.momentum = glm::vec2{momentum.at("x"), momentum.at("y")};
}

game::BulletController::BulletController () : EntityController("bullet") {}
