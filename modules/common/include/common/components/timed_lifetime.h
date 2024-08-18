#pragma once
#include "runtime/runtime.h"

namespace phenyl::component {
    class World;
}

namespace phenyl::common {
    struct TimedLifetime {
        double lifetime = 0.0;
        double livedTime = 0.0;

        static void Init (phenyl::runtime::PhenylRuntime& runtime);
    };
}