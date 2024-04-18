#include <cmath>
#include <iostream>

#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/components/audio_player.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/sprite.h>
#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/graphics/viewport.h>

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

static phenyl::InputAction GainUp;
static phenyl::InputAction GainDown;

static void updatePlayer (test::Player& player, phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::AudioPlayer& audioPlayer, phenyl::Viewport& viewport,
                          phenyl::Camera& camera);

void addPlayerComponents (test::TestApp* app) {
    app->addComponent<test::Player>();
}

void inputSetup (phenyl::GameInput& input) {
    //KeyLeft = input.mapInput("move_left", "key_a");
    KeyLeft = input.addAction("move_left");
    //KeyRight = input.mapInput("move_right", "key_d");
    KeyRight = input.addAction("move_right");
    //KeyUp = input.mapInput("move_up", "key_w");
    KeyUp = input.addAction("move_up");
    //KeyDown = input.mapInput("move_down", "key_s");
    KeyDown = input.addAction("move_down");

    //KeyShoot = input.mapInput("player_shoot", "mouse_left");
    KeyShoot = input.addAction("player_shoot");

    //GainUp = input.mapInput("gain_up", "key_up");
    GainUp = input.addAction("gain_up");
    //GainDown = input.mapInput("gain_down", "key_down");
    GainDown = input.addAction("gain_down");

    input.addActionBinding("move_left", "keyboard.key_a");
    input.addActionBinding("move_right", "keyboard.key_d");
    input.addActionBinding("move_up", "keyboard.key_w");
    input.addActionBinding("move_down", "keyboard.key_s");

    input.addActionBinding("player_shoot", "mouse.button_left");

    input.addActionBinding("gain_up", "keyboard.key_up");
    input.addActionBinding("gain_down", "keyboard.key_down");
}

void playerUpdate (phenyl::PhenylRuntime& runtime) {
    auto& viewport = runtime.resource<phenyl::Viewport>();
    auto& camera = runtime.resource<phenyl::Camera>();

    runtime.manager().query<test::Player, phenyl::GlobalTransform2D, phenyl::RigidBody2D, phenyl::AudioPlayer>().each([&camera, &viewport] (phenyl::Entity entity, test::Player& player, phenyl::GlobalTransform2D& transform,
            phenyl::RigidBody2D& body, phenyl::AudioPlayer& audioPlayer) {
        updatePlayer(player, transform, body, audioPlayer, viewport, camera);
    });
}


static glm::vec2 getMovementForce () {
    glm::vec2 forceVec{0.0f, 0.0f};

    if (KeyLeft.value()) {
        forceVec += glm::vec2{-FORCE_COMPONENT, 0.0f};
    }

    if (KeyRight.value()) {
        forceVec += glm::vec2{FORCE_COMPONENT, 0.0f};
    }

    if (KeyUp.value()) {
        forceVec += glm::vec2{0, FORCE_COMPONENT};
    }

    if (KeyDown.value()) {
        forceVec += glm::vec2{0, -FORCE_COMPONENT};
    }

    return forceVec;
}

static void updatePlayer (test::Player& player, phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::AudioPlayer& audioPlayer, phenyl::Viewport& viewport,
                          phenyl::Camera& camera) {
    auto forceVec = getMovementForce();
    auto disp = camera.getWorldPos2D(viewport.getCursorPos()) - transform.transform2D.position();
    bool doShoot = KeyShoot.value();

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

        audioPlayer.play(player.gunshotSample);

        player.hasShot = true;
    } else if (!doShoot && player.hasShot) {
        player.hasShot = false;
    }

    if (GainUp.value() && !player.gainPressed) {
        audioPlayer.setGain(audioPlayer.gain() + 0.1f);
        player.gainPressed = true;
        std::cout << "Player gain: " << audioPlayer.gain() << "\n";
    } else if (GainDown.value() && !player.gainPressed) {
        audioPlayer.setGain(audioPlayer.gain() - 0.1f);
        player.gainPressed = true;
        std::cout << "Player gain: " << audioPlayer.gain() << "\n";
    } else if (player.gainPressed && !GainUp.value() && !GainDown.value()) {
        player.gainPressed = false;
    }

    camera.setPos2D(transform.transform2D.position());
}