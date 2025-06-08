#include "core/components/2d/global_transform.h"

#include "core/components/3d/global_transform.h"
#include "core/serialization/serializer_impl.h"
#include "core/world.h"

namespace phenyl::core {
PHENYL_SERIALIZABLE(GlobalTransform2D)

void GlobalTransform2D::PropagateTransforms (const Bundle<const Transform2D, GlobalTransform2D>* parent,
    const Bundle<const Transform2D, GlobalTransform2D>& child) {
    auto& [childTransform, childGlobalTransform] = child.comps();
    if (parent) {
        auto& [_, parentGlobalTransform] = parent->comps();
        childGlobalTransform.transform =
            parentGlobalTransform.transform * static_cast<AffineTransform2D>(childTransform);
    } else {
        childGlobalTransform.transform = static_cast<AffineTransform2D>(childTransform);
    }
}

PHENYL_SERIALIZABLE(GlobalTransform3D, PHENYL_SERIALIZABLE_MEMBER(transform))
} // namespace phenyl::core
