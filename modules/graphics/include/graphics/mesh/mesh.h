#pragma once

#include "graphics/pipeline.h"

#include "graphics/buffer.h"
#include "util/hash.h"

#include "graphics/shader.h"

namespace phenyl::graphics {
    enum class MeshAttributeKind {
        Unknown = 0,
        Position,
        Normal,
        Color,
        TexCoord0,
        TexCoord1,
        TexCoord2,
        TexCoord3,
        TexCoord4,
        TexCoord5,
        TexCoord6,
        TexCoord7,
    };

    inline std::string GetMeshAttribName (MeshAttributeKind kind) {
        switch (kind) {
            case MeshAttributeKind::Unknown:
                break;
            case MeshAttributeKind::Position:
                return "position";
            case MeshAttributeKind::Normal:
                return "normal";
            case MeshAttributeKind::Color:
                return "color";
            case MeshAttributeKind::TexCoord0:
                return "texcoord_0";
            case MeshAttributeKind::TexCoord1:
                return "texcoord_1";
            case MeshAttributeKind::TexCoord2:
                return "texcoord_2";
            case MeshAttributeKind::TexCoord3:
                return "texcoord_3";
            case MeshAttributeKind::TexCoord4:
                return "texcoord_4";
            case MeshAttributeKind::TexCoord5:
                return "texcoord_5";
            case MeshAttributeKind::TexCoord6:
                return "texcoord_6";
            case MeshAttributeKind::TexCoord7:
                return "texcoord_7";
        }

        PHENYL_ABORT("Invalid mesh attribute kind!");
    }

    struct MeshAttribute {
        MeshAttributeKind kind = MeshAttributeKind::Unknown;
        ShaderDataType type = ShaderDataType::UNKNOWN;
        std::uint32_t stream = 0;
        std::size_t offset = 0;

        bool operator== (const MeshAttribute&) const = default;
    };

    struct MeshLayout {
        ShaderIndexType indexType;
        std::vector<MeshAttribute> attributes;
        std::vector<std::size_t> streamStrides;

        std::uint64_t layoutId = 0;

        bool operator== (const MeshLayout&) const = default;
    };

    class Mesh {
    private:
        MeshLayout meshLayout{};
        RawBuffer meshIndices;
        std::vector<RawBuffer> meshStreams;
        std::size_t meshSize;

    public:
        Mesh (MeshLayout layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams, std::size_t meshSize);

        const MeshLayout& layout () const noexcept {
            return meshLayout;
        }

        const RawBuffer& indices () const noexcept {
            return meshIndices;
        }

        const std::vector<RawBuffer>& streams () const noexcept {
            return meshStreams;
        }

        std::size_t numVertices () const noexcept {
            return meshSize;
        }

        // TODO
        void setLayoutId (std::uint64_t id) noexcept {
            meshLayout.layoutId = id;
        }
    };
}

template<>
struct std::hash<phenyl::graphics::MeshAttribute> {
    std::size_t operator() (const phenyl::graphics::MeshAttribute& attribute) const noexcept {
        return phenyl::util::HashAll(attribute.kind, attribute.type, attribute.stream, attribute.offset);
    }
};

template<>
struct std::hash<phenyl::graphics::MeshLayout> {
    std::size_t operator() (const phenyl::graphics::MeshLayout& layout) const noexcept {
        return phenyl::util::HashAll(layout.indexType, layout.attributes, layout.streamStrides);
    }
};