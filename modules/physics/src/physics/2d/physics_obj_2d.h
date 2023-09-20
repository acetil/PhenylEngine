#pragma once

#include "physics/physics.h"
#include "util/fl_vector.h"
#include "collisions_2d.h"


namespace phenyl::physics {
    class PhysicsObject2D : public IPhysics {
    private:
        void solveConstraints (std::vector<Constraint2D>& constraints, component::EntityComponentManager& compManager);
    public:
        void addComponents(component::EntityComponentManager& componentManager) override;
        void addComponentSerializers(component::EntitySerializer& serializer) override;
        void updatePhysics(component::EntityComponentManager& componentManager, float deltaTime) override;
        void checkCollisions(component::EntityComponentManager& componentManager, float deltaTime) override;

        void debugRender (const component::EntityComponentManager& componentManager) override;
    };
}