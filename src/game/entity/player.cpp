#include <cmath>

#include "engine/game_camera.h"
#include "physics/components/2D/rigid_body.h"
#include "common/components/2d/global_transform.h"

#include "game/entity/player.h"
#include "game/entity/serializers.h"
#include "component/component_serializer.h"
#include "common/assets/assets.h"

#define SHOOT_DIST (1.1f * 0.1f)
#define SHOOT_VEL 7.5f

#define FORCE_COMPONENT 6.5

static common::InputAction KeyLeft;
static common::InputAction KeyRight;
static common::InputAction KeyUp;
static common::InputAction KeyDown;

static common::InputAction KeyShoot;

static void updatePlayer (game::Player& player, common::GlobalTransform2D& transform, physics::RigidBody2D& body, game::GameInput& input,
                          game::GameCamera& camera);

void addPlayerComponents (component::ComponentManager& manager, component::EntitySerializer& serialiser) {
    manager.addComponent<game::Player>();

    serialiser.addSerializer<game::Player>();

}

void inputSetup (game::GameInput& input) {
    KeyLeft = input.getInput("move_left");
    KeyRight = input.getInput("move_right");
    KeyUp = input.getInput("move_up");
    KeyDown = input.getInput("move_down");

    KeyShoot = input.getInput("player_shoot");
}

void playerUpdate (component::ComponentManager& manager, game::GameInput& input, game::GameCamera& camera) {
    manager.query<game::Player, common::GlobalTransform2D, physics::RigidBody2D>().each([&camera, &input] (component::Entity entity, game::Player& player, common::GlobalTransform2D& transform, physics::RigidBody2D& body) {
        updatePlayer(player, transform, body, input, camera);
    });
}


static glm::vec2 getMovementForce (game::GameInput& input) {
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

    return forceVec;
}

static glm::vec2 getCursorDisp (game::GameInput& input, game::GameCamera& camera, glm::vec2 pos) {
    glm::vec2 pixelPos = input.cursorPos();
    glm::vec2 cursorPos = pixelPos / input.screenSize() * 2.0f - glm::vec2{1.0f, 1.0f};
    cursorPos.y *= -1;
    glm::vec2 worldCursorPos = camera.getWorldPos(cursorPos);

    return worldCursorPos - pos;
}

static void updatePlayer (game::Player& player, common::GlobalTransform2D& transform, physics::RigidBody2D& body, game::GameInput& input,
                          game::GameCamera& camera) {
    auto forceVec = getMovementForce(input);
    auto disp = getCursorDisp(input, camera, transform.transform2D.position());
    bool doShoot = input.isDown(KeyShoot);

    body.applyForce(forceVec * body.getMass());
    auto rot = std::atan2(disp.y, disp.x);
    transform.transform2D.setRotation(rot);

    if (doShoot && !player.hasShot) {
        glm::vec2 rotVec = glm::vec2{std::cos(rot), std::sin(rot)};
        glm::vec2 pos = rotVec * SHOOT_DIST + transform.transform2D.position();
        glm::vec2 bulletVel = rotVec * SHOOT_VEL;

        auto bulletView = player.bulletPrefab->instantiate()
                                .complete();

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

    camera.setPos(transform.transform2D.position());
}