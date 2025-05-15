#include "graphics/components/3d/mesh_renderer.h"

#include "core/assets/assets.h"
#include "core/serialization/serializer_impl.h"

using namespace phenyl::graphics;

namespace phenyl::graphics {
PHENYL_SERIALIZABLE(MeshRenderer3D, PHENYL_SERIALIZABLE_MEMBER(mesh), PHENYL_SERIALIZABLE_MEMBER(material))
}

// PHENYL_SERIALIZABLE(phenyl::graphics::MeshRenderer3D, PHENYL_SERIALIZABLE_MEMBER(mesh))
