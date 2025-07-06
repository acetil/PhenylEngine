#pragma once

#include "graphics/backend/renderer.h"
#include "graphics/maths_headers.h"
#include "graphics/mesh/mesh.h"

#include <iosfwd>
#include <vector>

namespace phenyl::graphics {
class ObjFile {
public:
    explicit ObjFile (std::istream& file);

    Mesh makeMesh (Renderer& renderer, bool includeW = false);

private:
    std::vector<glm::vec4> m_positions;
    std::vector<glm::vec2> m_uvs;
    std::vector<glm::vec3> m_normals;
    std::vector<std::uint32_t> m_indices;
};
} // namespace phenyl::graphics
