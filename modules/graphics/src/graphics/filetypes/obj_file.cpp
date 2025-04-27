#include "util/map.h"
#include "util/string_help.h"
#include "graphics/detail/loggers.h"

#include "obj_file.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"OBJ_LOADER", detail::GRAPHICS_LOGGER};

struct ObjFaceVertex {
    std::size_t v = -1;
    std::optional<std::size_t> vt;
    std::optional<std::size_t> vn;

    bool operator== (const ObjFaceVertex&) const = default;
};

template <>
struct std::hash<ObjFaceVertex> {
    std::size_t operator() (const ObjFaceVertex& v) const noexcept {
        return phenyl::util::HashAll(v.v, v.vt, v.vn);
    }
};

ObjFile::ObjFile (std::istream& file) {
    std::vector<glm::vec4> objVertices;
    std::vector<glm::vec2> objUvs;
    std::vector<glm::vec3> objNorms;
    std::vector<std::vector<ObjFaceVertex>> objFaces;

    std::string line;
    while (std::getline(file, line)) {
        auto trimmedLine = util::StringTrim(line);

        if (trimmedLine.empty()) {
            continue;
        }

        auto split = util::StringSplit(trimmedLine);
        auto it = split.begin();

        auto first = *it;
        ++it;

        if (first[0] == '#') {
            // comment
            continue;
        } else if (first == "v") {
            glm::vec4 v{0, 0, 0, 1};
            int i = 0;
            while (i < 4 && it != split.end()) {
                auto curr = *it;
                PHENYL_ASSERT_MSG(std::from_chars(curr.begin(), curr.end(), v[i]).ec == std::errc{}, "Failed to parse vertex component {}", curr);

                i++;
                ++it;
            }

            PHENYL_ASSERT_MSG(i >= 3, "Obj v directive requires at least 3 arguments!");
            objVertices.emplace_back(v);
        } else if (first == "vt") {
            glm::vec2 v{};

            int i = 0;
            while (i < 2 && it != split.end()) {
                auto curr = *it;
                PHENYL_ASSERT_MSG(std::from_chars(curr.begin(), curr.end(), v[i]).ec == std::errc{}, "Failed to parse vertex texture component {}", curr);

                i++;
                ++it;
            }

            PHENYL_ASSERT_MSG(i == 2, "Obj vt directive requires 2 arguments!");
            objUvs.emplace_back(v);
        } else if (first == "vn") {
            glm::vec3 v{};

            int i = 0;
            while (i < 3 && it != split.end()) {
                auto curr = *it;
                PHENYL_ASSERT_MSG(std::from_chars(curr.begin(), curr.end(), v[i]).ec == std::errc{}, "Failed to parse vertex normal component {}", curr);

                i++;
                ++it;
            }

            PHENYL_ASSERT_MSG(i == 3, "Obj vn directive requires 3 arguments!");
            objNorms.emplace_back(v);
        } else if (first == "f") {
            std::vector<ObjFaceVertex> faceVertices;
            for ( ; it != split.end(); ++it) {
                ObjFaceVertex f;
                auto curr = *it;

                int i = 0;
                for (auto v : util::StringSplit(curr, "/")) {
                    if (v.empty()) {
                        i++;
                        continue;
                    }

                    std::size_t index;
                    PHENYL_ASSERT_MSG(std::from_chars(v.begin(), v.end(), index).ec == std::errc{}, "Failed to parse face index {}", v);

                    if (i == 0) {
                        f.v = index - 1;
                    } else if (i == 1) {
                        f.vt = index - 1;
                    } else {
                        f.vn = index - 1;
                    }
                    i++;
                }

                faceVertices.emplace_back(f);
            }

            objFaces.emplace_back(std::move(faceVertices));
        } else {
            PHENYL_LOGD(LOGGER, "Skipping obj directive \"{}\"", first);
        }
    }

    PHENYL_ASSERT_MSG(!objFaces.empty(), "Obj files must have at least one face");
    bool hasTex = static_cast<bool>(objFaces[0][0].vt);
    bool hasNorm = static_cast<bool>(objFaces[0][0].vn);

    if (!std::all_of(objFaces.begin(), objFaces.end(), [&] (const std::vector<ObjFaceVertex>& f) {
        return std::all_of(f.begin(), f.end(), [&] (const ObjFaceVertex& v) {
            return static_cast<bool>(v.vn) == hasNorm && static_cast<bool>(v.vt) == hasTex;
        });
    })) {
        PHENYL_ABORT("Obj faces have inconsistent vertex and texture norms!");
    }

    util::HashMap<ObjFaceVertex, std::uint32_t> indexMap;

    for (const auto& f : objFaces) {
        for (const auto& v : f) {
            if (auto it = indexMap.find(v); it != indexMap.end()) {
                indices.emplace_back(it->second);
            } else {
                std::uint32_t index = positions.size();
                positions.emplace_back(objVertices[v.v]);

                if (hasTex) {
                    uvs.emplace_back(objUvs[*v.vt]);
                }

                if (hasNorm) {
                    normals.emplace_back(objNorms[*v.vn]);
                }

                indices.emplace_back(index);
                indexMap.emplace(v, index);
            }
        }
    }
}

std::unique_ptr<Mesh> ObjFile::makeMesh (Renderer& renderer, bool includeW) {
    MeshLayout layout{};

    std::size_t stride = 0;
    if (includeW) {
        stride = sizeof(glm::vec4);
        layout.attributes.emplace_back(MeshAttributeKind::Position, ShaderDataType::VEC4F, 0, 0);
    } else {
        stride = sizeof(glm::vec3);
        layout.attributes.emplace_back(MeshAttributeKind::Position, ShaderDataType::VEC3F, 0, 0);
    }

    std::size_t normOffset = 0;
    if (!normals.empty()) {
        normOffset = stride;
        layout.attributes.emplace_back(MeshAttributeKind::Normal, ShaderDataType::VEC3F, 0, normOffset);
        stride += sizeof(glm::vec3);
    }

    std::size_t uvOffset = 0;
    if (!uvs.empty()) {
        uvOffset = stride;
        layout.attributes.emplace_back(MeshAttributeKind::TexCoord0, ShaderDataType::VEC2F, 0, uvOffset);
        stride += sizeof(glm::vec2);
    }

    std::size_t numVertices = positions.size();
    auto data = std::make_unique<std::byte[]>(stride * numVertices);

    if (includeW) {
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec4*>(data.get() + i * stride);
            *ptr = positions[i];
        }
    } else {
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec3*>(data.get() + i * stride);
            *ptr = glm::vec3{positions[i]};
        }
    }
    if (!normals.empty()) {
        PHENYL_DASSERT(numVertices == normals.size());
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec3*>(data.get() + i * stride + normOffset);
            *ptr = normals[i];
        }
    }

    if (!uvs.empty()) {
        PHENYL_DASSERT(numVertices == uvs.size());
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec2*>(data.get() + i * stride + uvOffset);
            *ptr = uvs[i];
        }
    }

    auto vertexBuffer = renderer.makeRawBuffer(stride, numVertices, BufferStorageHint::STATIC);
    vertexBuffer.upload(data.get(), stride * numVertices);
    layout.streamStrides = std::vector{stride};

    RawBuffer indexBuffer;
    if (numVertices <= std::numeric_limits<std::uint16_t>::max()) {
        std::vector<std::uint16_t> vec{indices.begin(), indices.end()};
        indexBuffer = renderer.makeRawBuffer(sizeof(std::uint16_t), vec.size(), BufferStorageHint::STATIC, true);
        indexBuffer.upload(reinterpret_cast<std::byte*>(vec.data()), vec.size() * sizeof(std::uint16_t));

        layout.indexType = ShaderIndexType::USHORT;
    } else {
        indexBuffer = renderer.makeRawBuffer(sizeof(std::uint32_t), indices.size(), BufferStorageHint::STATIC, true);
        indexBuffer.upload(reinterpret_cast<std::byte*>(indices.data()), indices.size() * sizeof(std::uint32_t));

        layout.indexType = ShaderIndexType::UINT;
    }

    std::vector<RawBuffer> streams;
    streams.emplace_back(std::move(vertexBuffer));
    return std::make_unique<Mesh>(std::move(layout), std::move(indexBuffer), std::move(streams), indices.size());
}
