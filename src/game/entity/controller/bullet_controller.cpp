#include "bullet_controller.h"

#include "util/string_help.h"

void game::BulletController::onEntityCollision (view::EntityView &entityView, view::GameView& gameView, int otherEntityId, unsigned int layers) {
    gameView.destroyEntityInstance(entityView.entityId);
}

int game::BulletController::getTextureId (view::EntityView& entityView, view::GameView& gameView) {
    return texId;
}

void game::BulletController::setTextureIds (graphics::TextureAtlas& atlas) {
    texId = atlas.getModelId("test9");
}

void game::BulletController::initEntity (view::EntityView& entityView, view::GameView& gameView, std::string& opt) {
    glm::vec2 bulletVel;
    if (opt[0] == 'b') {
        util::stringToBin(std::string_view(opt.begin() + 1, opt.end()), bulletVel);
    } else {
        auto vec = util::stringSplit(opt);
        bulletVel.x = std::stof(vec[0]);
        bulletVel.y = std::stof(vec[1]);
    }
    entityView.velocity = bulletVel;
}
