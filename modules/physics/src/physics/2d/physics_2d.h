#pragma once

#include "physics/physics.h"
#include "util/fl_vector.h"
#include "collisions_2d.h"


namespace phenyl::physics {
    class Physics2D {
    private:
        void checkCollisions(component::EntityComponentManager& componentManager, float deltaTime);
        void solveConstraints (std::vector<Constraint2D>& constraints, component::EntityComponentManager& compManager);
    public:
        void addComponents(runtime::PhenylRuntime& runtime);
        void updatePhysics(component::EntityComponentManager& componentManager, float deltaTime);

        void debugRender (component::EntityComponentManager& componentManager);
    };
}