#include <phenyl/components/audio_player.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/components/2D/particle_emitter.h>

#include <phenyl/signals/physics.h>

#include "paddle.h"
#include "breakout.h"

using namespace breakout;

static phenyl::InputAction LeftKey;
static phenyl::InputAction RightKey;

static phenyl::InputAction BallShoot;

void breakout::initPaddle (breakout::BreakoutApp* app, phenyl::GameInput& input, phenyl::ComponentManager& manager) {
    app->addComponent<Paddle>();

    LeftKey = input.mapInput("move_left", "key_a");
    RightKey = input.mapInput("move_right", "key_d");
    BallShoot = input.mapInput("ball_shoot", "mouse_left");

    manager.handleSignal<phenyl::signals::OnCollision, const Paddle, phenyl::AudioPlayer>([] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, const Paddle& paddle, phenyl::AudioPlayer& audioPlayer) {
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

void breakout::updatePaddle (float deltaTime, phenyl::ComponentManager& manager, phenyl::GameInput& input, phenyl::Camera& camera) {
    manager.query<const phenyl::GlobalTransform2D, phenyl::RigidBody2D, Paddle>().each([&input, &camera, deltaTime] (auto entity, const phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, Paddle& paddle) {
        paddle.update(deltaTime, entity, transform, body, input, camera);
    });
}

void Paddle::update (float deltaTime, phenyl::Entity entity, const phenyl::GlobalTransform2D& transform, phenyl::RigidBody2D& body, phenyl::GameInput& input, phenyl::Camera& camera) {
    glm::vec2 direction{};
    if (input.isDown(LeftKey)) {
        direction += glm::vec2{-1.0f, 0.0f};
    }

    if (input.isDown(RightKey)) {
        direction += glm::vec2{1.0f, 0.0f};
    }

    auto vel = direction * speed;
    auto newPos = vel * deltaTime + transform.transform2D.position();

    vel.x = (glm::clamp(newPos.x, minX + width / 2, maxX - width / 2) - transform.transform2D.position().x) / deltaTime;

    body.applyImpulse(vel * body.getMass() - body.getMomentum());

    if (input.isDown(BallShoot) && hasBall) {
        hasBall = false;
        auto pos = transform.transform2D.position() + glm::vec2{0, 0.1};

        auto mousePos = camera.getWorldPos2D(input.cursorPos() / input.screenSize() * 2.0f - glm::vec2{1.0f, 1.0f});
        mousePos.y *= -1;

        auto ballVel = /*vel + */glm::normalize(mousePos - pos) * ballSpeed;
        ballPrefab->instantiate()
            .complete()
            .apply<phenyl::GlobalTransform2D>([pos] (phenyl::GlobalTransform2D& transform) {
                transform.transform2D.setPosition(pos);
            })
            .apply<phenyl::RigidBody2D>([ballVel] (phenyl::RigidBody2D& body) {
                body.applyImpulse(body.getMass() * ballVel);
            });
    } else if (!input.isDown(BallShoot) && !hasBall) {
        hasBall = true;
    }
}
