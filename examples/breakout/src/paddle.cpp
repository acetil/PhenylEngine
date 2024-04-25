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

void breakout::initPaddle (breakout::BreakoutApp* app, phenyl::PhenylRuntime& runtime) {
    app->addComponent<Paddle>();

    auto& input = runtime.resource<phenyl::GameInput>();

    PlayerMove = input.addAxis2D("player_move");
    CursorPos = input.addAxis2D("cursor_pos");
    
    BallShoot = input.addAction("ball_shoot");

    input.addButtonAxis2DBinding("player_move", "keyboard.key_a", glm::vec2{-1.0f, 0.0f});
    input.addButtonAxis2DBinding("player_move", "keyboard.key_d", glm::vec2{1.0f, 0.0f});
    input.addAxis2DBinding("cursor_pos", "mouse.mouse_pos");
    input.addActionBinding("ball_shoot", "mouse.button_left");

    runtime.manager().handleSignal<phenyl::signals::OnCollision, const Paddle, phenyl::AudioPlayer>([] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, const Paddle& paddle, phenyl::AudioPlayer& audioPlayer) {
        phenyl::GlobalTransform2D emitterTransform{};
        emitterTransform.transform2D.setPosition(signal.worldContactPoint);

        paddle.emitterPrefab->instantiate()
              .with(emitterTransform)
              .complete()
              .apply<phenyl::ParticleEmitter2D>([normal = signal.normal] (phenyl::ParticleEmitter2D& emitter) {
                emitter.direction = normal;
            });
        audioPlayer.play(paddle.bounceSample);
    });
}

void breakout::updatePaddle (float deltaTime, phenyl::PhenylRuntime& runtime) {
    auto& camera = runtime.resource<phenyl::Camera>();

    runtime.manager().query<const phenyl::GlobalTransform2D, phenyl::RigidBody2D, Paddle>().each([&camera, deltaTime] (auto entity, const phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, Paddle& paddle) {
        paddle.update(deltaTime, entity, transform, body, camera);
    });
}

void Paddle::update (float deltaTime, phenyl::Entity entity, const phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::Camera& camera) {
    auto vel = PlayerMove.value() * speed;
    auto newPos = vel * deltaTime + transform.transform2D.position();

    vel.x = (glm::clamp(newPos.x, minX + width / 2, maxX - width / 2) - transform.transform2D.position().x) / deltaTime;

    body.applyImpulse(vel * body.getMass() - body.getMomentum());

    if (BallShoot.value() && hasBall) {
        hasBall = false;
        auto pos = transform.transform2D.position() + glm::vec2{0, 0.1};

        auto mousePos = camera.getWorldPos2D(CursorPos.value());
        auto ballVel = /*vel + */glm::normalize(mousePos - pos) * ballSpeed;
        ballPrefab->instantiate()
            .complete()
            .apply<phenyl::GlobalTransform2D>([pos] (phenyl::GlobalTransform2D& transform) {
                transform.transform2D.setPosition(pos);
            })
            .apply<phenyl::RigidBody2D>([ballVel] (phenyl::RigidBody2D& body) {
                body.applyImpulse(body.getMass() * ballVel);
            });
    } else if (!BallShoot.value() && !hasBall) {
        hasBall = true;
    }
}
