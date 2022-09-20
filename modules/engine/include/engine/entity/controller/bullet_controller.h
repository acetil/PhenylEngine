#pragma once

#include "engine/entity/controller/entity_controller.h"

namespace game {
    class BulletController : public EntityController {
    public:
        BulletController();
        void onEntityCollision(component::EntityView &entityView, view::GameView& gameView, component::EntityView& otherEntity, unsigned int layers) override;
        void initEntity (component::EntityView& entityView, view::GameView& gameView, const util::DataValue& data) override;
    };
}
