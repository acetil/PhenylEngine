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

PHENYL_SERIALIZABLE(GlobalTransform3D)

Quaternion GlobalTransform3D::rotation () const {
    glm::mat3 mat = {
      glm::normalize(glm::vec3{transform[0]}),
      glm::normalize(glm::vec3{transform[1]}),
      glm::normalize(glm::vec3{transform[2]}),
    };

    return Quaternion{mat};
}

void GlobalTransform3D::PropagateTransforms (const Bundle<const Transform3D, GlobalTransform3D>* parent,
    const Bundle<const Transform3D, GlobalTransform3D>& child) {
    auto& [childTransform, childGlobalTransform] = child.comps();
    if (parent) {
        auto& [_, parentGlobalTransform] = parent->comps();
        childGlobalTransform.transform = parentGlobalTransform.transform * childTransform.transformMatrx();
    } else {
        childGlobalTransform.transform = childTransform.transformMatrx();
    }
}

} // namespace phenyl::core
