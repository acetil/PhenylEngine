#include "core/plugins/core_plugin_3d.h"

#include "core/components/3d/global_transform.h"
#include "core/maths/3d/transform.h"
#include "core/plugins/core_plugin.h"
#include "core/runtime.h"

using namespace phenyl::core;

std::string_view Core3DPlugin::getName () const noexcept {
    return "Core3DPlugin";
}

void Core3DPlugin::init (PhenylRuntime& runtime) {
    runtime.addPlugin<CorePlugin>();

    runtime.addComponent<Transform3D>("Transform3D");
    runtime.addComponent<GlobalTransform3D>("GlobalTransform3D");

    runtime.addHierarchicalSystem<PostUpdate>("GlobalTransform3D::PropagateTransforms",
        &GlobalTransform3D::PropagateTransforms);
}
