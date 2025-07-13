#include "obj_file.h"

#include "graphics/detail/loggers.h"
#include "util/string_help.h"

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
                PHENYL_ASSERT_MSG(std::from_chars(curr.begin(), curr.end(), v[i]).ec == std::errc{},
                    "Failed to parse vertex component {}", curr);

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
                PHENYL_ASSERT_MSG(std::from_chars(curr.begin(), curr.end(), v[i]).ec == std::errc{},
                    "Failed to parse vertex texture component {}", curr);

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
                PHENYL_ASSERT_MSG(std::from_chars(curr.begin(), curr.end(), v[i]).ec == std::errc{},
                    "Failed to parse vertex normal component {}", curr);

                i++;
                ++it;
            }

            PHENYL_ASSERT_MSG(i == 3, "Obj vn directive requires 3 arguments!");
            objNorms.emplace_back(v);
        } else if (first == "f") {
            std::vector<ObjFaceVertex> faceVertices;
            for (; it != split.end(); ++it) {
                ObjFaceVertex f;
                auto curr = *it;

                int i = 0;
                for (auto v : util::StringSplit(curr, "/")) {
                    if (v.empty()) {
                        i++;
                        continue;
                    }

                    std::size_t index;
                    PHENYL_ASSERT_MSG(std::from_chars(v.begin(), v.end(), index).ec == std::errc{},
                        "Failed to parse face index {}", v);

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
                m_indices.emplace_back(it->second);
            } else {
                std::uint32_t index = m_positions.size();
                m_positions.emplace_back(objVertices[v.v]);

                if (hasTex) {
                    m_uvs.emplace_back(objUvs[*v.vt]);
                }

                if (hasNorm) {
                    m_normals.emplace_back(objNorms[*v.vn]);
                }

                m_indices.emplace_back(index);
                indexMap.emplace(v, index);
            }
        }
    }
}

std::shared_ptr<Mesh> ObjFile::makeMesh (Renderer& renderer, bool includeW) {
    MeshLayout layout{};
    MeshBuilder builder{renderer};

    std::size_t stride = 0;
    if (includeW) {
        stride = sizeof(glm::vec4);
        // layout.attributes.emplace_back(MeshAttributeKind::Position, ShaderDataType::VEC4F, 0, 0);
        builder.withAttribute<glm::vec4>(MeshAttributeKind::Position, 0);
    } else {
        stride = sizeof(glm::vec3);
        // layout.attributes.emplace_back(MeshAttributeKind::Position, ShaderDataType::VEC3F, 0, 0);
        builder.withAttribute<glm::vec3>(MeshAttributeKind::Position, 0);
    }

    std::size_t normOffset = 0;
    if (!m_normals.empty()) {
        normOffset = stride;
        // layout.attributes.emplace_back(MeshAttributeKind::Normal, ShaderDataType::VEC3F, 0, normOffset);
        builder.withAttribute<glm::vec3>(MeshAttributeKind::Normal, 0, normOffset);
        stride += sizeof(glm::vec3);
    }

    std::size_t uvOffset = 0;
    if (!m_uvs.empty()) {
        uvOffset = stride;
        // layout.attributes.emplace_back(MeshAttributeKind::TexCoord0, ShaderDataType::VEC2F, 0, uvOffset);
        builder.withAttribute<glm::vec2>(MeshAttributeKind::TexCoord0, 0, uvOffset);
        stride += sizeof(glm::vec2);
    }

    std::size_t numVertices = m_positions.size();
    auto data = std::make_unique<std::byte[]>(stride * numVertices);

    if (includeW) {
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec4*>(data.get() + i * stride);
            *ptr = m_positions[i];
        }
    } else {
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec3*>(data.get() + i * stride);
            *ptr = glm::vec3{m_positions[i]};
        }
    }
    if (!m_normals.empty()) {
        PHENYL_DASSERT(numVertices == m_normals.size());
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec3*>(data.get() + i * stride + normOffset);
            *ptr = m_normals[i];
        }
    }

    if (!m_uvs.empty()) {
        PHENYL_DASSERT(numVertices == m_uvs.size());
        for (std::size_t i = 0; i < numVertices; i++) {
            auto* ptr = reinterpret_cast<glm::vec2*>(data.get() + i * stride + uvOffset);
            *ptr = m_uvs[i];
        }
    }
    builder.withStream({data.get(), stride * numVertices}, stride);

    // auto vertexBuffer = renderer.makeRawBuffer(stride, numVertices, BufferStorageHint::STATIC);
    // vertexBuffer.upload({data.get(), stride * numVertices});
    // layout.streamStrides = std::vector{stride};
    //
    // RawBuffer indexBuffer;
    // if (numVertices <= std::numeric_limits<std::uint16_t>::max()) {
    //     std::vector<std::uint16_t> vec{m_indices.begin(), m_indices.end()};
    //     indexBuffer = renderer.makeRawBuffer(sizeof(std::uint16_t), vec.size(), BufferStorageHint::STATIC, true);
    //     indexBuffer.upload(std::as_bytes(std::span{vec}));
    //
    //     layout.indexType = ShaderIndexType::USHORT;
    // } else {
    //     indexBuffer = renderer.makeRawBuffer(sizeof(std::uint32_t), m_indices.size(), BufferStorageHint::STATIC,
    //     true); indexBuffer.upload(std::as_bytes(std::span{m_indices}));
    //
    //     layout.indexType = ShaderIndexType::UINT;
    // }
    //
    // std::vector<RawBuffer> streams;
    // streams.emplace_back(std::move(vertexBuffer));
    // return Mesh{std::move(layout), std::move(indexBuffer), std::move(streams), m_indices.size()};

    if (numVertices <= std::numeric_limits<std::uint16_t>::max()) {
        std::vector<std::uint16_t> vec{m_indices.begin(), m_indices.end()};
        builder.withIndices(std::span<const std::uint16_t>{vec});
    } else {
        // indexBuffer = renderer.makeRawBuffer(sizeof(std::uint32_t), m_indices.size(), BufferStorageHint::STATIC,
        // true); indexBuffer.upload(std::as_bytes(std::span{m_indices}));
        builder.withIndices(std::span<const std::uint32_t>{m_indices});
        //
        // layout.indexType = ShaderIndexType::UINT;
    }
    return builder.build();
}
