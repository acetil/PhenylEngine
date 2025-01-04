#pragma once

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

    struct MeshAttribute {
        MeshAttributeKind kind = MeshAttributeKind::Unknown;
        ShaderDataType type = ShaderDataType::UNKNOWN;
        std::uint32_t stream = 0;
        std::size_t offset = 0;
    };

    struct MeshLayout {
        ShaderDataType indexType;
        std::vector<MeshAttribute> attributes;
        std::uint32_t numStreams;
    };

    class Mesh {
    private:
        MeshLayout meshLayout{};
        RawBuffer meshIndices;
        std::vector<RawBuffer> meshStreams;

    public:
        Mesh (MeshLayout layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams);

        const MeshLayout& layout () const noexcept {
            return meshLayout;
        }

        const RawBuffer& indices () const noexcept {
            return meshIndices;
        }

        const std::vector<RawBuffer>& streams () const noexcept {
            return meshStreams;
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
        return phenyl::util::HashAll(layout.indexType, layout.attributes, layout.numStreams);
    }
};