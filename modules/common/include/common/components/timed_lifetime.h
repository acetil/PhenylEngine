#pragma once

namespace phenyl::component {
    class ComponentManager;
}

namespace phenyl::common {
    struct TimedLifetime {
        double lifetime;
        double livedTime = 0.0;

        static void Update (phenyl::component::ComponentManager& componentManager, double deltaTime);
    };
}