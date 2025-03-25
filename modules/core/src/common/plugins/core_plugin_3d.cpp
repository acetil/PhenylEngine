#include "core/plugins/core_plugin_3d.h"
#include "core/runtime.h"
#include "core/components/3d/global_transform.h"
#include "core/maths/3d/transform.h"

using namespace phenyl::core;

std::string_view Core3DPlugin::getName () const noexcept {
    return "Core3DPlugin";
}

void Core3DPlugin::init (PhenylRuntime& runtime) {
    runtime.addComponent<GlobalTransform3D>("GlobalTransform3D");
}
