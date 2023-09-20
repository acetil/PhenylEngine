#pragma once

#include "component/component.h"
#include "component/forward.h"

namespace phenyl::physics {



    class IPhysics {
    public:
        virtual ~IPhysics() = default;
        virtual void addComponents (component::EntityComponentManager& componentManager) = 0;
        virtual void addComponentSerializers (component::EntitySerializer& serializer) = 0;
        virtual void updatePhysics (component::EntityComponentManager& componentManager, float deltaTime) = 0;
        virtual void checkCollisions (component::EntityComponentManager& componentManager, float deltaTime) = 0;

        virtual void debugRender (const component::EntityComponentManager& componentManager) = 0;
    };

    std::unique_ptr<IPhysics> makeDefaultPhysics ();
}
