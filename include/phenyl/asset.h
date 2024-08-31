#pragma once

#include "common/assets/assets.h"

namespace phenyl {
    template <typename T>
    using Asset = phenyl::common::Asset<T>;

    template <typename T>
    using AssetManager = phenyl::common::AssetManager<T>;

    using Assets = phenyl::common::Assets;
}