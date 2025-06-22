#include "paddle.h"

#include "breakout.h"

#include <iostream>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/particle_emitter.h>
#include <phenyl/components/audio_player.h>
#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/signals/physics.h>

using namespace breakout;

namespace breakout {
PHENYL_SERIALIZABLE(Paddle, PHENYL_SERIALIZABLE_MEMBER_NAMED(hasBall, "has_ball"), PHENYL_SERIALIZABLE_MEMBER(speed),
    PHENYL_SERIALIZABLE_MEMBER(width), PHENYL_SERIALIZABLE_MEMBER_NAMED(minX, "min_x"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(maxX, "max_x"), PHENYL_SERIALIZABLE_MEMBER_NAMED(ballPrefab, "ball_prefab"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(ballSpeed, "ball_speed"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(emitterPrefab, "emitter_prefab"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(bounceSample, "bounce_sample"));
}

static phenyl::Axis2DInput PlayerMove;
static phenyl::Axis2DInput CursorPos;

static phenyl::InputAction BallShoot;

void breakout::InitPaddle (breakout::BreakoutApp* app, phenyl::PhenylRuntime& runtime) {
    app->addComponent<Paddle>("Paddle");

    auto& input = runtime.resource<phenyl::GameInput>();

    PlayerMove = input.addAxis2D("player_move");
    CursorPos = input.addAxis2D("cursor_pos");

    BallShoot = input.addAction("ball_shoot");

    input.addButtonAxis2DBinding("player_move", "keyboard.key_a", glm::vec2{-1.0f, 0.0f});
    input.addButtonAxis2DBinding("player_move", "keyboard.key_d", glm::vec2{1.0f, 0.0f});
    input.addAxis2DBinding("cursor_pos", "mouse.mouse_pos");
    input.addActionBinding("ball_shoot", "mouse.button_left");

    runtime.world().addHandler<phenyl::signals::OnCollision, const Paddle, phenyl::AudioPlayer>(
        [] (const phenyl::signals::OnCollision& signal,
            const phenyl::Bundle<const Paddle, phenyl::AudioPlayer>& bundle) {
            auto& [paddle, audioPlayer] = bundle.comps();
            auto& world = bundle.entity().world();

            auto emitterEntity = world.create();
            emitterEntity.insert(phenyl::Transform2D{}.setPosition(signal.worldContactPoint));
            paddle.emitterPrefab->instantiate(emitterEntity);
            emitterEntity.apply<phenyl::ParticleEmitter2D>(
                [normal = signal.normal] (phenyl::ParticleEmitter2D& emitter) { emitter.direction = normal; });

            // paddle.emitterPrefab->instantiate()
            //       .with(emitterTransform)
            //       .complete()
            //       .apply<phenyl::ParticleEmitter2D>([normal = signal.normal]
            //       (phenyl::ParticleEmitter2D& emitter) {
            //         emitter.direction = normal;
            //     });
            audioPlayer.play(paddle.bounceSample);
        });
    runtime.addSystem<phenyl::FixedUpdate>("Paddle::Update", &Paddle::update);
}

void Paddle::update (const phenyl::Resources<const phenyl::Camera2D, const phenyl::FixedDelta>& resources,
    const phenyl::Bundle<const phenyl::GlobalTransform2D, phenyl::RigidBody2D>& bundle) {
    auto& [transform, body] = bundle.comps();
    auto& [camera, deltaTime] = resources;

    auto vel = PlayerMove.value() * speed;
    auto newPos = vel * static_cast<float>(deltaTime()) + transform.position();

    vel.x = (glm::clamp(newPos.x, minX + width / 2, maxX - width / 2) - transform.position().x) /
        static_cast<float>(deltaTime());

    body.applyImpulse(vel * body.mass() - body.momentum());

    if (BallShoot.value() && hasBall) {
        hasBall = false;
        auto pos = transform.position() + glm::vec2{0, 0.1};

        auto mousePos = camera.getWorldPos2D(CursorPos.value());
        auto ballVel = /*vel + */ glm::normalize(mousePos - pos) * ballSpeed;
        /*ballPrefab->instantiate()
            .complete()
            .apply<phenyl::GlobalTransform2D>([pos] (phenyl::GlobalTransform2D& transform) {
                transform.transform2D.setPosition(pos);
            })
            .apply<phenyl::RigidBody2D>([ballVel] (phenyl::RigidBody2D& body) {
                body.applyImpulse(body.getMass() * ballVel);
            });*/

        auto ballEntity = bundle.entity().world().create();
        ballPrefab->instantiate(ballEntity);
        ballEntity.apply<phenyl::Transform2D>([pos] (phenyl::Transform2D& transform) { transform.setPosition(pos); });
        ballEntity.apply<phenyl::RigidBody2D>(
            [ballVel] (phenyl::RigidBody2D& body) { body.applyImpulse(body.mass() * ballVel); });

    } else if (!BallShoot.value() && !hasBall) {
        hasBall = true;
    }
}
