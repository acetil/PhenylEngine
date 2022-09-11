#include "engine/entity/controller/wall_controller.h"
#include "physics/collision_component.h"

#include "util/string_help.h"


using namespace game;

void WallController::initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    if (data.empty() || data.get<util::DataObject>().empty()) {
        return;
    }

    auto dataObj = data.get<util::DataObject>();

    if (dataObj.contains("no_coll") && dataObj.at("no_coll").is<bool>() && dataObj.at("no_coll").get<bool>()) {
        entityView.removeComponent<physics::CollisionComponent2D>();
        return;
    }

    util::DataObject size = dataObj.at("size");

    entityView.getComponent<graphics::Transform2D>().ifPresent([&size] (graphics::Transform2D& comp) {
        comp.transform = glm::mat2{{size.at("x").get<float>(), 0.0f}, {0.0f, size.at("y").get<float>()}} * comp.transform;
        comp.rotTransform = comp.transform;
    });

    entityView.getComponent<physics::CollisionComponent2D>().ifPresent([&dataObj, &size] (physics::CollisionComponent2D& comp) {
        if (dataObj.contains("htibox_size")) {
            util::DataObject hitboxSize = dataObj.at("hitbox_size");
            comp.bbMap = glm::mat2{{hitboxSize.at("x"), 0.0f}, {0.0f, hitboxSize.at("y")}} * comp.bbMap;
        } else {
            comp.bbMap = glm::mat2{{size.at("x").get<float>(), 0.0f}, {0.0f, size.at("y").get<float>()}} * comp.bbMap;
        }

        auto vec = comp.bbMap[0] + comp.bbMap[1];
        comp.outerRadius = glm::sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
        comp.rotBBMap = comp.bbMap;
    });
    //entityView.modelScale.scaleBy(size.at("x"), size.at("y"));

    /*if (dataObj.contains("hitbox_size")) {
        util::DataObject hitboxSize = dataObj.at("hitbox_size");
        entityView.hitboxScale.scaleBy(hitboxSize.at("x"), hitboxSize.at("y"));
    } else {
        entityView.hitboxScale.scaleBy(size.at("x"), size.at("y"));
    }*/
}

util::DataObject WallController::getData (component::EntityView& entityView, view::GameView& gameView) {
    return EntityController::getData(entityView, gameView);
}
