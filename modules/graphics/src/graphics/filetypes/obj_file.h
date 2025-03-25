#pragma once

#include <iosfwd>
#include <vector>
#include "graphics/backend/renderer.h"
#include "graphics/mesh/mesh.h"

#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    class ObjFile {
    private:
        std::vector<glm::vec4> positions;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<std::uint32_t> indices;

    public:
        explicit ObjFile (std::istream& file);

        std::unique_ptr<Mesh> makeMesh (Renderer& renderer, bool includeW = false);
    };
}
