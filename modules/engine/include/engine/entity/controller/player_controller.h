#pragma once

#include "event/events/player_movement_change.h"
#include "engine/entity/controller/entity_controller.h"
#include "event/events/cursor_position_change.h"
#include "event/events/player_shoot_change.h"

namespace game {
    class PlayerController : public EntityController {
    private:
        float deltaXForce;
        float deltaYForce;
        glm::vec2 cursorScreenPos = {0, 0};
        bool doShoot = false;
        bool hasShot = false;
        int texId = 0;
    public:
        PlayerController () {
            deltaXForce = 0;
            deltaYForce = 0;
        }
        void updateMovement (event::PlayerMovementChangeEvent& event);
        void controlEntityPrePhysics (view::EntityView& entityView, view::GameView& gameView) override;
        void controlEntityPostPhysics(view::EntityView &entityView, view::GameView &gameView) override;
        void updateCursorPos (event::CursorPosChangeEvent& event);
        void updateDoShoot (event::PlayerShootChangeEvent& event);
        int getTextureId (view::EntityView& entityView, view::GameView& gameView) override;
        void setTextureIds (graphics::TextureAtlas& atlas) override;
    };
}
