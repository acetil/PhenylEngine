#pragma once

#include "core/assets/asset.h"

#include "graphics/mesh/mesh.h"

namespace phenyl::graphics {
    struct MeshRenderer3D {
        core::Asset<Mesh> mesh;
    };

    PHENYL_DECLARE_SERIALIZABLE(MeshRenderer3D)
}
