#pragma once

#include "core/runtime.h"
#include "core/serialization/serializer_forward.h"

namespace phenyl::core {
struct TimedLifetime {
    double lifetime = 0.0;
    double livedTime = 0.0;

    static void Init (PhenylRuntime& runtime);
};

PHENYL_DECLARE_SERIALIZABLE(TimedLifetime);
} // namespace phenyl::core
