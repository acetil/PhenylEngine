#pragma once

#include "engine/application.h"
#include "engine/plugins/default_2d_plugin.h"

namespace phenyl {
    template <engine::PluginType ...Plugins>
    using Application = phenyl::engine::Application<Plugins...>;
    using Application2D = Application<engine::Default2DPlugin>;

    using ApplicationProperties = phenyl::engine::ApplicationProperties;
}