#pragma once

#include "engine/entity/controller/entity_controller.h"

namespace game {
    class WallController : public EntityController {
    private:
        int texId;
    public:
        void controlEntityPrePhysics (component::view::EntityView& entityView, view::GameView& gameView) override {};
        void onEntityCollision (component::view::EntityView& entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers) override {}; // TODO: hit/hurtbox
        int getTextureId (component::view::EntityView& entityView, view::GameView& gameView) const override {
            return texId;
        }
        void setTextureIds (graphics::TextureAtlas& atlas) override {
            texId = atlas.getModelId("test10");
        }

        void initEntity (component::view::EntityView& entityView, view::GameView&, const util::DataValue& data) override; // TODO: convert to binary format
        util::DataObject getData(component::view::EntityView &entityView, view::GameView &gameView) override;
    };
}
