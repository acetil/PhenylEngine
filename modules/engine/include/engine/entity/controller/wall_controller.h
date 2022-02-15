#pragma once

#include "engine/entity/controller/entity_controller.h"

namespace game {
    class WallController : public EntityController {
    private:
        int texId;
    public:
        void controlEntityPrePhysics (view::EntityView& entityView, view::GameView& gameView) override {};
        void onEntityCollision (view::EntityView& entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers) override {}; // TODO: hit/hurtbox
        int getTextureId (view::EntityView& entityView, view::GameView& gameView) override {
            return texId;
        }
        void setTextureIds (graphics::TextureAtlas& atlas) override {
            texId = atlas.getModelId("test10");
        }

        void initEntity (view::EntityView& entityView, view::GameView&, const util::DataValue& data) override; // TODO: convert to binary format
        util::DataObject getData(view::EntityView &entityView, view::GameView &gameView) override;
    };
}
