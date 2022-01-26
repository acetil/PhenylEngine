#include "engine/entity/controller/wall_controller.h"

#include "util/string_help.h"


using namespace game;

void WallController::initEntity (view::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    if (data.empty() || data.get<util::DataObject>().empty()) {
        return;
    }
    auto dataObj = data.get<util::DataObject>();

    util::DataObject size = dataObj.at("size");

    entityView.modelScale.scaleBy(size.at("x"), size.at("y"));

    if (dataObj.contains("hitbox_size")) {
        util::DataObject hitboxSize = dataObj.at("hitbox_size");
        entityView.hitboxScale.scaleBy(hitboxSize.at("x"), hitboxSize.at("y"));
    } else {
        entityView.hitboxScale.scaleBy(size.at("x"), size.at("y"));
    }
}

util::DataObject WallController::getData (view::EntityView& entityView, view::GameView& gameView) {
    return EntityController::getData(entityView, gameView);
}
