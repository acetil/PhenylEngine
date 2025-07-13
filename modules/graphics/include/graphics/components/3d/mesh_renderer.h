#pragma once

#include "core/assets/asset.h"
#include "graphics/material.h"
#include "graphics/mesh/mesh.h"

namespace phenyl::graphics {
struct MeshRenderer3D {
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<MaterialInstance> material;
};

PHENYL_DECLARE_SERIALIZABLE(MeshRenderer3D)
} // namespace phenyl::graphics
