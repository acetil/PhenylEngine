#pragma once

#include <phenyl/asset.h>
#include <phenyl/audio.h>
#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/physics/2D/rigid_body.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/prefab.h>
#include <phenyl/runtime.h>
#include <phenyl/serialization.h>
#include <phenyl/world.h>

namespace breakout {
class BreakoutApp;

struct Paddle {
    bool hasBall = true;
    float speed = 1.0f;
    float width = 1.0f;
    float minX = 0.0f;
    float maxX = 1.0f;
    float ballSpeed = 1.0f;
    std::shared_ptr<phenyl::Prefab> ballPrefab;
    std::shared_ptr<phenyl::Prefab> emitterPrefab;
    std::shared_ptr<phenyl::AudioSample> bounceSample;

    void update (const phenyl::Resources<const phenyl::Camera2D, const phenyl::FixedDelta>& resources,
        const phenyl::Bundle<const phenyl::GlobalTransform2D, phenyl::RigidBody2D>& bundle);
};
PHENYL_DECLARE_SERIALIZABLE(Paddle)

void InitPaddle (BreakoutApp* app, phenyl::PhenylRuntime& runtime);
} // namespace breakout
