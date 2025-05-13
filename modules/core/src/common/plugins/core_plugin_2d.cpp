#include "core/plugins/core_plugin_2d.h"

#include "core/components/2d/global_transform.h"
#include "core/runtime.h"

using namespace phenyl::core;

std::string_view Core2DPlugin::getName () const noexcept {
    return "Core2DPlugin";
}

void Core2DPlugin::init (PhenylRuntime& runtime) {
    runtime.addComponent<GlobalTransform2D>("GlobalTransform2D");
}
