#include <cmath>
#include <iostream>

#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/components/audio_player.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/sprite.h>
#include <phenyl/components/physics/2D/rigid_body.h>

#include "player.h"
#include "game/test_app.h"

#define SHOOT_DIST (1.1f * 0.1f)
#define SHOOT_VEL 7.5f

#define FORCE_COMPONENT 6.5f

using namespace phenyl;

namespace test {
    PHENYL_SERIALIZABLE(Player,
        PHENYL_SERIALIZABLE_MEMBER_NAMED(bulletPrefab, "bullet_prefab"),
        PHENYL_SERIALIZABLE_MEMBER_NAMED(gunshotSample, "gunshot_sample")
    )
}

static phenyl::Axis2DInput PlayerMove;

static phenyl::Axis2DInput CursorPos;
static phenyl::InputAction KeyShoot;

static phenyl::InputAction GainUp;
static phenyl::InputAction GainDown;

static void PlayerFixedUpdateSystem (const Resources<const Camera>& resources, const phenyl::Bundle<test::Player, phenyl::GlobalTransform2D, phenyl::RigidBody2D, phenyl::AudioPlayer>& bundle);
static void PlayerCameraUpdateSystem (const Resources<Camera>& resources, const test::Player& player, const phenyl::GlobalTransform2D& transform);
static void PlayerAudioUpdateSystem (test::Player& player, phenyl::AudioPlayer& audioPlayer);

void InputSetup (phenyl::GameInput& input) {
    PlayerMove = input.addAxis2D("player_move", true);
    CursorPos = input.addAxis2D("cursor_pos");
    KeyShoot = input.addAction("player_shoot");

    GainUp = input.addAction("gain_up");
    GainDown = input.addAction("gain_down");

    input.addButtonAxis2DBinding("player_move", "keyboard.key_a", glm::vec2{-1, 0});
    input.addButtonAxis2DBinding("player_move", "keyboard.key_d", glm::vec2{1, 0});
    input.addButtonAxis2DBinding("player_move", "keyboard.key_w", glm::vec2{0, 1});
    input.addButtonAxis2DBinding("player_move", "keyboard.key_s", glm::vec2{0, -1});

    input.addAxis2DBinding("cursor_pos", "mouse.mouse_pos");
    input.addActionBinding("player_shoot", "mouse.button_left");

    input.addActionBinding("gain_up", "keyboard.key_up");
    input.addActionBinding("gain_down", "keyboard.key_down");
}

void InitPlayer (test::TestApp* app) {
    app->addComponent<test::Player>("Player");

    InputSetup(app->runtime().resource<GameInput>());

    app->runtime().addSystem<FixedUpdate>("Player::FixedUpdate", PlayerFixedUpdateSystem);
    app->runtime().addSystem<Update>("Player::CameraUpdate", PlayerCameraUpdateSystem);
    app->runtime().addSystem<Update>("Player::AudioUpdate", PlayerAudioUpdateSystem);

}

static void PlayerFixedUpdateSystem (const Resources<const Camera>& resources, const phenyl::Bundle<test::Player, phenyl::GlobalTransform2D, phenyl::RigidBody2D, phenyl::AudioPlayer>& bundle) {
    auto& [camera] = resources;
    auto& [player, transform, body, audioPlayer] = bundle.comps();

    auto forceVec = PlayerMove.value() * FORCE_COMPONENT;
    auto disp = camera.getWorldPos2D(CursorPos.value()) - transform.transform2D.position();
    bool doShoot = KeyShoot.value();

    body.applyForce(forceVec * body.getMass());
    auto rot = std::atan2(disp.y, disp.x);
    transform.transform2D.setRotation(rot);

    if (doShoot && !player.hasShot) {
        glm::vec2 rotVec = glm::vec2{std::cos(rot), std::sin(rot)};
        glm::vec2 pos = rotVec * SHOOT_DIST + transform.transform2D.position();
        glm::vec2 bulletVel = rotVec * SHOOT_VEL;

        auto bulletEntity = bundle.entity().world().create();
        player.bulletPrefab->instantiate(bulletEntity);

        bulletEntity.apply<phenyl::GlobalTransform2D>([pos, rot] (auto& transform) {
            transform.transform2D
                     .setPosition(pos)
                     .setRotation(rot);
        });
        bulletEntity.apply<phenyl::RigidBody2D>([bulletVel] (auto& body) {
            body.applyImpulse(bulletVel * body.getMass());
        });

        // bulletEntity.apply<phenyl::GlobalTransform2D, phenyl::RigidBody2D>([pos, bulletVel, rot] (phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body) {
        //     transform.transform2D
        //              .setPosition(pos)
        //              .setRotation(rot);
        //
        //     body.applyImpulse(bulletVel * body.getMass());
        // });

        audioPlayer.play(player.gunshotSample);

        player.hasShot = true;
    } else if (!doShoot && player.hasShot) {
        player.hasShot = false;
    }
}

static void PlayerCameraUpdateSystem (const Resources<Camera>& resources, const test::Player& player, const phenyl::GlobalTransform2D& transform) {
    auto& [camera] = resources;
    camera.setPos2D(transform.transform2D.position());
}

static void PlayerAudioUpdateSystem (test::Player& player, phenyl::AudioPlayer& audioPlayer) {
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
}