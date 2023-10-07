#pragma once

#include <phenyl/asset.h>
#include <phenyl/prefab.h>
#include "phenyl/component.h"

namespace breakout {
    class BreakoutApp;

    struct Wall {
        phenyl::Asset<phenyl::Prefab> emitter;
    };

    PHENYL_SERIALIZE(Wall, {
        PHENYL_MEMBER(emitter);
    })

    void initWall (BreakoutApp* app, phenyl::ComponentManager& manager);
}