#pragma once

#include "core/assets/asset.h"
#include "graphics/material.h"
#include "graphics/mesh/mesh.h"

namespace phenyl::graphics {
struct MeshRenderer3D {
    core::Asset<Mesh> mesh;
    core::Asset<MaterialInstance> material;
};

PHENYL_DECLARE_SERIALIZABLE(MeshRenderer3D)
} // namespace phenyl::graphics
