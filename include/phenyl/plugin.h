#pragma once

#include "core/plugin.h"

namespace phenyl {
using IInitPlugin = core::IInitPlugin;
using IPlugin = core::IPlugin;

template <typename T> concept PluginType =
    std::derived_from<T, core::IPlugin> || std::derived_from<T, core::IInitPlugin>;
} // namespace phenyl
