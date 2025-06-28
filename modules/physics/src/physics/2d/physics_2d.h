#pragma once

#include "core/debug.h"
#include "core/world.h"
#include "physics/physics.h"

namespace phenyl::physics {
class Physics2D {
public:
    void addComponents (core::PhenylRuntime& runtime);

    void debugRender (core::World& world, core::Debug& debug);
};
} // namespace phenyl::physics
