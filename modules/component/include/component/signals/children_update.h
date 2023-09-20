#pragma once

#include "component/entity.h"

namespace phenyl::component {
    struct OnAddChild {
        Entity child;
    };

    struct OnRemoveChild {
        Entity child;
    };
}