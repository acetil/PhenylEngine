#include <phenyl/components/audio_player.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/components/2D/particle_emitter.h>

#include <phenyl/signals/physics.h>

#include "paddle.h"

#include <iostream>

#include "breakout.h"

using namespace breakout;

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

    runtime.manager().addHandler<phenyl::signals::OnCollision, const Paddle, phenyl::AudioPlayer>([] (const phenyl::signals::OnCollision& signal, const phenyl::Bundle<const Paddle, phenyl::AudioPlayer>& bundle) {
        auto& [paddle, audioPlayer] = bundle.comps();
        auto& manager = bundle.entity().manager();

        phenyl::GlobalTransform2D emitterTransform{};
        emitterTransform.transform2D.setPosition(signal.worldContactPoint);

        auto emitterEntity = manager.create();
        emitterEntity.insert(emitterTransform);
        paddle.emitterPrefab->instantiate(emitterEntity);
        emitterEntity.apply<phenyl::ParticleEmitter2D>([normal = signal.normal] (phenyl::ParticleEmitter2D& emitter) {
            emitter.direction = normal;
        });

        // paddle.emitterPrefab->instantiate()
        //       .with(emitterTransform)
        //       .complete()
        //       .apply<phenyl::ParticleEmitter2D>([normal = signal.normal] (phenyl::ParticleEmitter2D& emitter) {
        //         emitter.direction = normal;
        //     });
        audioPlayer.play(paddle.bounceSample);
    });
    runtime.addSystem<phenyl::FixedUpdate>("Paddle::Update", &Paddle::update);
}

void Paddle::update (const phenyl::Resources<const phenyl::Camera, const phenyl::FixedDelta>& resources, const phenyl::Bundle<const phenyl::GlobalTransform2D, phenyl::RigidBody2D>& bundle) {
    auto& [transform, body] = bundle.comps();
    auto& [camera, deltaTime] = resources;

    auto vel = PlayerMove.value() * speed;
    auto newPos = vel * static_cast<float>(deltaTime()) + transform.transform2D.position();

    vel.x = (glm::clamp(newPos.x, minX + width / 2, maxX - width / 2) - transform.transform2D.position().x) / static_cast<float>(deltaTime());

    body.applyImpulse(vel * body.getMass() - body.getMomentum());

    if (BallShoot.value() && hasBall) {
        hasBall = false;
        auto pos = transform.transform2D.position() + glm::vec2{0, 0.1};

        auto mousePos = camera.getWorldPos2D(CursorPos.value());
        auto ballVel = /*vel + */glm::normalize(mousePos - pos) * ballSpeed;
        /*ballPrefab->instantiate()
            .complete()
            .apply<phenyl::GlobalTransform2D>([pos] (phenyl::GlobalTransform2D& transform) {
                transform.transform2D.setPosition(pos);
            })
            .apply<phenyl::RigidBody2D>([ballVel] (phenyl::RigidBody2D& body) {
                body.applyImpulse(body.getMass() * ballVel);
            });*/

        auto ballEntity = bundle.entity().manager().create();
        ballPrefab->instantiate(ballEntity);
        ballEntity.apply<phenyl::GlobalTransform2D>([pos] (phenyl::GlobalTransform2D& transform) {
            transform.transform2D.setPosition(pos);
        });
        ballEntity.apply<phenyl::RigidBody2D>([ballVel] (phenyl::RigidBody2D& body) {
            body.applyImpulse(body.getMass() * ballVel);
        });

    } else if (!BallShoot.value() && !hasBall) {
        hasBall = true;
    }
}
