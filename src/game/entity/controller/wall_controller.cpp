#include "wall_controller.h"

#include "util/string_help.h"


using namespace game;

void WallController::initEntity (view::EntityView& entityView, view::GameView& gameView, std::string& opts) {
    logging::log(LEVEL_DEBUG, "Created wall entity!");
    glm::vec2 pos = entityView.position();

    auto sizes = util::stringSplit(opts, ",");
    if (sizes.size() >= 2) {
        float xSize = std::stof(sizes[0]);
        float ySize = std::stof(sizes[1]);

        entityView.modelScale.scaleBy(xSize, ySize);
        entityView.hitboxScale.scaleBy(xSize, ySize);
    }
}