#include "runtime/runtime.h"

#include "common/components/2d/global_transform_serialize.h"
#include "common/plugins/core_plugin_2d.h"

using namespace phenyl::common;

std::string_view Core2DPlugin::getName () const noexcept {
    return "Core2DPlugin";
}

void Core2DPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addComponent<GlobalTransform2D>("GlobalTransform2D");
}