#pragma once

#include <phenyl/asset.h>
#include <phenyl/audio.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/prefab.h>
#include <phenyl/runtime.h>
#include <phenyl/serialization.h>
#include <phenyl/world.h>

namespace test {
class TestApp;

struct Player {
    std::shared_ptr<phenyl::Prefab> bulletPrefab;
    std::shared_ptr<phenyl::AudioSample> gunshotSample;

    bool hasShot{false};
    bool gainPressed{false};
};

PHENYL_DECLARE_SERIALIZABLE(Player)
} // namespace test

void InitPlayer (test::TestApp* app);
void InputSetup (phenyl::GameInput& input);
