#pragma once

#include "core/entity.h"

namespace phenyl::core {
struct OnAddChild {
    Entity child;
};

struct OnRemoveChild {
    Entity child;
};
} // namespace phenyl::core
