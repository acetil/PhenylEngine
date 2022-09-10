#pragma once

#include "engine/entity/controller/entity_controller.h"

namespace game {
    class BulletController : public EntityController {
    public:
        void onEntityCollision(component::view::EntityView &entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers) override;
        //int getTextureId (component::view::EntityView& entityView, view::GameView& gameView) const override;
        //void setTextureIds (graphics::TextureAtlas& atlas) override;
        void initEntity (component::view::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) override;
    };
}
