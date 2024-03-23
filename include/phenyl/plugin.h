#pragma once

#include "runtime/init_plugin.h"
#include "runtime/plugin.h"

namespace phenyl {
    using IInitPlugin = runtime::IInitPlugin;
    using IPlugin = runtime::IPlugin;

    template <typename T>
    concept PluginType = std::derived_from<T, runtime::IPlugin> || std::derived_from<T, runtime::IInitPlugin>;
}