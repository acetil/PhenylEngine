#pragma once

#include "physics/physics.h"
#include "util/fl_vector.h"
#include "collisions_2d.h"


namespace phenyl::physics {
    class Physics2D {
    private:
    public:
        void addComponents(runtime::PhenylRuntime& runtime);

        void debugRender (component::World& world);
    };
}