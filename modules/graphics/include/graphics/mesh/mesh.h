#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/buffer.h"
#include "graphics/backend/pipeline.h"
#include "graphics/backend/renderer.h"
#include "graphics/backend/shader.h"
#include "util/hash.h"

#include <queue>
#include <unordered_set>

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

inline constexpr std::string_view GetMeshAttribName (MeshAttributeKind kind) {
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

class Meshes;

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
} // namespace phenyl::graphics

template <>
struct std::hash<phenyl::graphics::MeshAttribute> {
    std::size_t operator() (const phenyl::graphics::MeshAttribute& attribute) const noexcept {
        return phenyl::util::HashAll(attribute.kind, attribute.type, attribute.stream, attribute.offset);
    }
};

template <>
struct std::hash<phenyl::graphics::MeshLayout> {
    std::size_t operator() (const phenyl::graphics::MeshLayout& layout) const noexcept {
        return phenyl::util::HashAll(layout.indexType, layout.attributes, layout.streamStrides);
    }
};

namespace phenyl::graphics {
class Mesh : public core::Asset<Mesh> {
public:
    Mesh (const MeshLayout* layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams, std::size_t meshSize);

    const MeshLayout& layout () const noexcept {
        return *m_layout;
    }

    const RawBuffer& indices () const noexcept {
        return m_indices;
    }

    const std::vector<RawBuffer>& streams () const noexcept {
        return m_streams;
    }

    std::size_t numVertices () const noexcept {
        return m_size;
    }

    void bind (Pipeline& pipeline, const std::vector<BufferBinding>& streamBindings) const;

    static std::string_view GetAssetType ();

private:
    const MeshLayout* m_layout;
    RawBuffer m_indices;
    std::vector<RawBuffer> m_streams;
    std::size_t m_size;
};

class Meshes : public core::AssetManager<Mesh>, public core::IResource {
public:
    class Builder {
    public:
        Builder& withStream (std::span<const std::byte> data, std::size_t stride);
        Builder& withAttribute (MeshAttribute attribute);
        Builder& withIndices (ShaderIndexType indexType, std::span<const std::byte> data, std::size_t size);
        Builder& withSize (std::size_t size);

        template <typename T>
        Builder& withAttribute (MeshAttributeKind kind, std::uint32_t stream, std::size_t offset = 0) {
            return withAttribute(MeshAttribute{
              .kind = kind,
              .type = GetShaderDataType<T>(),
              .stream = stream,
              .offset = offset,
            });
        }

        template <typename T>
        Builder& withVertices (MeshAttributeKind kind, std::span<const T> data) {
            std::uint32_t stream = m_streams.size();
            withStream(std::as_bytes(data), sizeof(T));
            withAttribute<T>(kind, stream);
            if (!m_indexBuffer) {
                withSize(data.size());
            }
            return *this;
        }

        template <typename T>
        Builder& withIndices (std::span<const T> indices) {
            return withIndices(GetIndexType<T>(), std::as_bytes(indices), indices.size());
        }

        std::shared_ptr<Mesh> build ();

    private:
        Meshes& m_meshes;
        Renderer& m_renderer;
        std::vector<RawBuffer> m_streams;
        std::vector<std::size_t> m_streamStrides;
        std::optional<RawBuffer> m_indexBuffer;
        std::optional<ShaderIndexType> m_indexType;

        std::vector<MeshAttribute> m_attribs;

        std::optional<std::size_t> m_size;

        Builder (Meshes& meshes, Renderer& renderer);

        friend class Meshes;
    };

    explicit Meshes (Renderer& renderer);

    Builder builder ();

    std::shared_ptr<Mesh> load (core::AssetLoadContext& ctx) override;
    void selfRegister ();
    std::string_view getName () const noexcept override;

private:
    Renderer& m_renderer;

    std::unordered_set<MeshLayout> m_layouts;
    std::uint64_t m_nextLayoutId = 1;

    const MeshLayout& findLayout (MeshLayout&& layout);
};
} // namespace phenyl::graphics

template <>
struct std::formatter<phenyl::graphics::MeshAttributeKind, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format string for MeshAttributeKind");
        }

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format (const phenyl::graphics::MeshAttributeKind& kind, FmtContext& ctx) const {
        return std::format_to(ctx.out(), "{}", phenyl::graphics::GetMeshAttribName(kind));
    }
};
