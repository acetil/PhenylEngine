#include "graphics/mesh/mesh.h"

#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"MESH", detail::GRAPHICS_LOGGER};

Mesh::Mesh (MeshLayout layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams, std::size_t meshSize) :
    m_layout{std::move(layout)},
    m_indices{std::move(meshIndices)},
    m_streams{std::move(meshStreams)},
    m_size{meshSize} {}

MeshBuilder::MeshBuilder (Renderer& renderer) : m_renderer{renderer} {}

MeshBuilder& MeshBuilder::withStream (std::span<const std::byte> data, std::size_t stride) {
    auto& stream = m_streams.emplace_back(m_renderer.makeRawBuffer(stride, data.size(), BufferStorageHint::STATIC));
    m_streamStrides.emplace_back(stride);
    stream.upload(data);
    return *this;
}

MeshBuilder& MeshBuilder::withAttribute (MeshAttribute attribute) {
    // if (attribute.stream >= m_streams.size()) {
    //     PHENYL_LOGE(LOGGER, "Stream index {} is larger than number of streams ({}), ignoring.", attribute.stream,
    //         m_streams.size());
    //     return *this;
    // }
    //
    // if (attribute.offset >= m_streamStrides[attribute.stream]) {
    //     PHENYL_LOGE(LOGGER, "Attribute offset {} is larger than stream stride {}, ignoring", attribute.offset,
    //         m_streamStrides[attribute.stream]);
    //     return *this;
    // }

    if (std::ranges::find_if(m_attribs, [&] (const auto& attrib) { return attrib.kind == attribute.kind; }) !=
        m_attribs.end()) {
        PHENYL_LOGE(LOGGER, "Attribute {} has already been added, ignoring", attribute.kind);
        return *this;
    }

    m_attribs.emplace_back(attribute);
    return *this;
}

MeshBuilder& MeshBuilder::withIndices (ShaderIndexType indexType, std::span<const std::byte> data, std::size_t size) {
    PHENYL_ASSERT_MSG(!m_indexBuffer, "Indices already exist");
    PHENYL_DASSERT(size <= data.size() / GetIndexTypeSize(indexType));
    m_indexBuffer = m_renderer.makeRawBuffer(GetIndexTypeSize(indexType), data.size(), BufferStorageHint::STATIC, true);

    m_indexBuffer->upload(data);
    m_indexType = indexType;
    m_size = size;
    return *this;
}

MeshBuilder& MeshBuilder::withSize (std::size_t size) {
    m_size = m_size ? std::min(*m_size, size) : size;
    return *this;
}

std::shared_ptr<Mesh> MeshBuilder::build () {
    // TODO
    PHENYL_ASSERT_MSG(m_indexBuffer, "Mesh must have index buffer (TODO)");
    PHENYL_ASSERT_MSG(m_size, "Mesh must have size");
    PHENYL_DASSERT(m_streamStrides.size() == m_streams.size());

    MeshLayout layout{.indexType = *m_indexType,
      .attributes = std::move(m_attribs),
      .streamStrides = std::move(m_streamStrides)};
    return std::make_shared<Mesh>(std::move(layout), std::move(*m_indexBuffer), std::move(m_streams), *m_size);
}
