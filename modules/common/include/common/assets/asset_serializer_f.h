#pragma once

#include "asset_serializer.h"

namespace phenyl::common {
    template <typename T>
    phenyl_AssetSerializer<T> phenyl_serialization_obj(Asset<T>*);
}