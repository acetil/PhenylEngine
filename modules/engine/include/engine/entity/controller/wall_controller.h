#pragma once

#include "engine/entity/controller/entity_controller.h"
#include "graphics/textures/texture_atlas.h"

namespace game {
    class WallController : public EntityController {
    private:
        //int texId;
    public:
        WallController ();
        void controlEntityPrePhysics (component::EntityView& entityView, view::GameView& gameView) override {};
        void onEntityCollision (component::EntityView& entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers) override {}; // TODO: hit/hurtbox

        void initEntity (component::EntityView& entityView, view::GameView&, const util::DataValue& data) override; // TODO: convert to binary format
        util::DataObject getData(component::EntityView &entityView, view::GameView &gameView) override;
    };
}
