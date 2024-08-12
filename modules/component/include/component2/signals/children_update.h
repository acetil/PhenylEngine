#pragma once

#include "component2/entity2.h"

namespace phenyl::component {
    struct OnAddChild2 {
        Entity2 child;
    };

    struct OnRemoveChild2 {
        Entity2 child;
    };
}