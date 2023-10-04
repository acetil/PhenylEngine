#include <cmath>

#include <phenyl/input.h>
#include <phenyl/engine.h>
#include <phenyl/component.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/physics/2D/rigid_body.h>

#include "player.h"
#include "serializers.h"
#include "game/test_app.h"

#define SHOOT_DIST (1.1f * 0.1f)
#define SHOOT_VEL 7.5f

#define FORCE_COMPONENT 6.5

using namespace phenyl;

static phenyl::InputAction KeyLeft;
static phenyl::InputAction KeyRight;
static phenyl::InputAction KeyUp;
static phenyl::InputAction KeyDown;

static phenyl::InputAction KeyShoot;

static void updatePlayer (test::Player& player, phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::GameInput& input,
                          phenyl::GameCamera& camera);

void addPlayerComponents (test::TestApp* app) {
    app->addComponent<test::Player>();
}

void inputSetup (phenyl::GameInput& input) {
    KeyLeft = input.mapInput("move_left", "key_a");
    KeyRight = input.mapInput("move_right", "key_d");
    KeyUp = input.mapInput("move_up", "key_w");
    KeyDown = input.mapInput("move_down", "key_s");

    KeyShoot = input.mapInput("player_shoot", "mouse_left");
}

void playerUpdate (phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::GameCamera& camera) {
    manager.query<test::Player, phenyl::GlobalTransform2D, phenyl::RigidBody2D>().each([&camera, &input] (phenyl::Entity entity, test::Player& player, phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body) {
        updatePlayer(player, transform, body, input, camera);
    });
}


static glm::vec2 getMovementForce (phenyl::GameInput& input) {
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

static glm::vec2 getCursorDisp (phenyl::GameInput& input, phenyl::GameCamera& camera, glm::vec2 pos) {
    glm::vec2 pixelPos = input.cursorPos();
    glm::vec2 cursorPos = pixelPos / input.screenSize() * 2.0f - glm::vec2{1.0f, 1.0f};
    cursorPos.y *= -1;
    glm::vec2 worldCursorPos = camera.getWorldPos(cursorPos);

    return worldCursorPos - pos;
}

static void updatePlayer (test::Player& player, phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::GameInput& input,
                          phenyl::GameCamera& camera) {
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

        bulletView.apply<phenyl::GlobalTransform2D, phenyl::RigidBody2D>([pos, bulletVel, rot] (phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body) {
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