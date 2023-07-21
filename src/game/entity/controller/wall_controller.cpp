#include "engine/entity/controller/wall_controller.h"
#include "physics/components/2D/collision_component.h"

#include "util/string_help.h"
#include "common/components/2d/global_transform.h"


using namespace game;

void WallController::initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) {
    if (data.empty() || data.get<util::DataObject>().empty()) {
        return;
    }

    auto dataObj = data.get<util::DataObject>();

    if (dataObj.contains("no_coll") && dataObj.at("no_coll").is<bool>() && dataObj.at("no_coll").get<bool>()) {
        //entityView.erase<physics::CollisionComponent2D>();
        return;
    }

    auto size = dataObj.at("size").get<glm::vec2>();

    entityView.get<common::GlobalTransform2D>().ifPresent([&size] (common::GlobalTransform2D& comp) {
        /*comp.transform = glm::mat2{{size.at("x").get<float>(), 0.0f}, {0.0f, size.at("y").get<float>()}} * comp.transform;
        comp.rotTransform = comp.transform;*/

        comp.transform2D.setScale(size);
    });

    entityView.get<physics::CollisionComponent2D>().ifPresent([&dataObj, &size, &gameView] (physics::CollisionComponent2D& comp) {
        if (dataObj.contains("hitbox_size")) {
            util::DataObject hitboxSize = dataObj.at("hitbox_size");
            //comp.bbMap = glm::mat2{{hitboxSize.at("x"), 0.0f}, {0.0f, hitboxSize.at("y")}} * comp.bbMap;
            comp.transform = glm::mat2{{hitboxSize.at("x"), 0.0f}, {0.0f, hitboxSize.at("y")}};
        } else {
            //comp.bbMap = glm::mat2{{size.at("x").get<float>(), 0.0f}, {0.0f, size.at("y").get<float>()}} * comp.bbMap;
            comp.transform = glm::mat2{{size.x, 0.0f}, {0.0f, size.y}} * comp.transform;
        }
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

WallController::WallController () : EntityController("wall_entity") {

}
