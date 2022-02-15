#pragma once

#include "engine/entity/controller/entity_controller.h"

namespace game {
    class BulletController : public EntityController {
    private:
        int texId = 0;
    public:
        void onEntityCollision(view::EntityView &entityView, view::GameView& gameView, component::EntityId otherEntityId, unsigned int layers) override;
        int getTextureId (view::EntityView& entityView, view::GameView& gameView) override;
        void setTextureIds (graphics::TextureAtlas& atlas) override;
        void initEntity (view::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) override;
    };
}
