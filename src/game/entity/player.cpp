#include <cmath>

#include "engine/game_camera.h"
#include "physics/components/2D/rigid_body.h"
#include "common/components/2d/global_transform.h"

#include "game/entity/player.h"
#include "game/entity/serializers.h"
#include "component/component_serializer.h"
#include "common/events/cursor_position_change.h"

#define SHOOT_DIST (1.1f * 0.1f)
#define SHOOT_VEL 7.5f

#define FORCE_COMPONENT 6.5

static glm::vec2 cursorPos{0.0f, 0.0f};

static common::InputAction KeyLeft;
static common::InputAction KeyRight;
static common::InputAction KeyUp;
static common::InputAction KeyDown;

static common::InputAction KeyShoot;

static void onCursorChange (event::CursorPosChangeEvent& event) {
    cursorPos = event.screenPos;
}

void addPlayerComponents (component::ComponentManager& manager, component::EntitySerializer& serialiser) {
    manager.addComponent<game::Player>();

    serialiser.addSerializer<game::Player>();

}

void inputSetup (game::GameInput& input, const event::EventBus::SharedPtr& eventBus) {
    eventBus->subscribeUnscoped(onCursorChange);

    KeyLeft = input.getInput("move_left");
    KeyRight = input.getInput("move_right");
    KeyUp = input.getInput("move_up");
    KeyDown = input.getInput("move_down");

    KeyShoot = input.getInput("player_shoot");
}

void playerUpdate (component::ComponentManager& manager, game::GameInput& input, game::PhenylGame& object) {
    glm::vec2 forceVec{0.0f, 0.0f};

    if (input.isDown(KeyLeft)) {
        forceVec += glm::vec2{-FORCE_COMPONENT, 0.0f};
    }

    if (input.isDown(KeyRight)) {
        forceVec += glm::vec2{FORCE_COMPONENT, 0.0f};
    }

    if (input.isDown(KeyUp)) {
        forceVec += glm::vec2{0, FORCE_COMPONENT};
    }

    if (input.isDown(KeyDown)) {
        forceVec += glm::vec2{0, -FORCE_COMPONENT};
    }

    glm::vec2 worldCursorPos = object.getCamera().getWorldPos(cursorPos);

    manager.query<game::Player, common::GlobalTransform2D, physics::RigidBody2D>().each([forceVec, worldCursorPos] (component::Entity entity, game::Player& player, common::GlobalTransform2D& transform, physics::RigidBody2D& body) {
        body.applyForce(forceVec * body.getMass());

        auto disp = worldCursorPos - transform.transform2D.position();
        auto rot = std::atan2(disp.y, disp.x);
        transform.transform2D.setRotation(rot);
    });
}

void playerUpdatePost (component::ComponentManager& manager, game::GameInput& input, game::PhenylGame& game) {
    glm::vec2 worldCursorPos = game.getCamera().getWorldPos(cursorPos);
    bool doShoot = input.isDown(KeyShoot);

    manager.query<game::Player, common::GlobalTransform2D>().each([doShoot, &game] (component::Entity entity, game::Player& player, common::GlobalTransform2D& transform) {
        if (doShoot && !player.hasShot) {
            auto rot = transform.transform2D.rotationAngle();

            glm::vec2 rotVec = glm::vec2{std::cos(rot), std::sin(rot)};
            glm::vec2 pos = rotVec * SHOOT_DIST + transform.transform2D.position();
            glm::vec2 bulletVel = rotVec * SHOOT_VEL;

            auto bulletView = game.createNewEntityInstance("bullet_entity");

            bulletView.apply<common::GlobalTransform2D, physics::RigidBody2D>([pos, bulletVel, rot] (common::GlobalTransform2D& transform, physics::RigidBody2D& body) {
                transform.transform2D
                         .setPosition(pos)
                         .setRotation(rot);

                body.applyImpulse(bulletVel * body.getMass());
            });

            player.hasShot = true;
        } else if (!doShoot && player.hasShot) {
            player.hasShot = false;
        }

        game.getCamera().setPos(transform.transform2D.position());
    });
}