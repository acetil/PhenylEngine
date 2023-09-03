#pragma once

#include "engine/entity/controller/entity_controller.h"

namespace game {
    class BulletController : public EntityController {
    public:
        BulletController();
        void onEntityCollision(component::Entity &entityView, view::GameView& gameView, component::Entity& otherEntity, unsigned int layers) override;
        void initEntity (component::Entity& entityView, view::GameView& gameView, const util::DataValue& data) override;
    };
}
