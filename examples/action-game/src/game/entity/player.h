#pragma once

#include <phenyl/audio.h>
#include <phenyl/asset.h>
#include <phenyl/component.h>
#include <phenyl/engine.h>
#include <phenyl/input.h>
#include <phenyl/prefab.h>
#include <phenyl/runtime.h>

namespace test {
    class TestApp;
    struct Player {
        phenyl::Asset<phenyl::Prefab> bulletPrefab;
        phenyl::Asset<phenyl::AudioSample> gunshotSample;

        bool hasShot{false};
        bool gainPressed{false};
    };
}

void InitPlayer (test::TestApp* app);
void InputSetup (phenyl::GameInput& input);