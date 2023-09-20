#pragma once

#include <memory>

#include "component/forward.h"

namespace physics {
    class IPhysics;
    class PhenylPhysics {
    private:
        IPhysics* physicsObj;
    public:
        explicit PhenylPhysics (IPhysics* physicsObj);
        void updatePhysics (component::EntityComponentManager& componentManager, float deltaTime);
        void checkCollisions (component::EntityComponentManager& componentManager, float deltaTime);
        void debugRender (const component::EntityComponentManager& componentManager);
    };
}