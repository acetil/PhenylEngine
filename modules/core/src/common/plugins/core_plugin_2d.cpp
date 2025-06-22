#include "core/plugins/core_plugin_2d.h"

#include "core/components/2d/global_transform.h"
#include "core/components/3d/global_transform.h"
#include "core/runtime.h"

using namespace phenyl::core;

namespace {
// void PropagateTransform (const Bundle<const Transform2D, GlobalTransform2D>* parent, const Bundle<const Transform2D,
// GlobalTransform2D>& child) {
//     auto& [childTransform, childGlobalTransform] = child.comps();
//     if (parent) {
//         // TODO
//     } else {
//         childGlobalTransform.transform = static_cast<AffineTransform2D>(childTransform);
//     }
// }
}

std::string_view Core2DPlugin::getName () const noexcept {
    return "Core2DPlugin";
}

void Core2DPlugin::init (PhenylRuntime& runtime) {
    runtime.addComponent<Transform2D>("Transform2D");
    runtime.addComponent<GlobalTransform2D>("GlobalTransform2D");

    runtime.addHierarchicalSystem<PostUpdate>("GlobalTransform2D::PropagateTransforms",
        &GlobalTransform2D::PropagateTransforms);
}
