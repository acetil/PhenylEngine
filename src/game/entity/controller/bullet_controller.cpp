#include "engine/entity/controller/bullet_controller.h"
#include "graphics/textures/texture_atlas.h"
#include "physics/components/simple_friction.h"
#include "physics/components/2D/kinematic_motion.h"

#include "util/string_help.h"

void game::BulletController::onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.getId());
}

/*int game::BulletController::getTextureId (component::EntityView& entityView, view::GameView& gameView) const {
    return texId;
}

void game::BulletController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test9");
}*/

void game::BulletController::initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    const auto& velocity = data.get<util::DataObject>().at("velocity").get<util::DataObject>();
    entityView.getComponent<physics::KinematicMotion2D>().ifPresent([&velocity] (physics::KinematicMotion2D& comp) {
       comp.velocity = glm::vec2{velocity.at("x"), velocity.at("y")};
    });
    //entityView.velocity = glm::vec2{velocity.at("x"), velocity.at("y")};
}

game::BulletController::BulletController () : EntityController("bullet") {}
