#pragma once

#include "common/serializer.h"
#include "timed_lifetime.h"

namespace phenyl::common {
    PHENYL_SERIALIZE(TimedLifetime, {
        PHENYL_MEMBER(lifetime);
    })
}