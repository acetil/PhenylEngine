#pragma once

#include "core/assets/assets.h"

namespace phenyl {
template<typename T>
using Asset = phenyl::core::Asset<T>;

template<typename T>
using AssetManager = phenyl::core::AssetManager<T>;

using Assets = phenyl::core::Assets;
} // namespace phenyl
