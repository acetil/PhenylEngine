#pragma once

#include "component/entity.h"

namespace component {
    struct OnAddChild {
        Entity child;
    };

    struct OnRemoveChild {
        Entity child;
    };
}