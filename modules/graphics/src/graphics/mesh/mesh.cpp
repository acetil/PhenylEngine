#include "graphics/mesh/mesh.h"

#include "core/assets/assets.h"
#include "graphics/detail/loggers.h"
#include "graphics/filetypes/obj_file.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"MESH", detail::GRAPHICS_LOGGER};

Mesh::Mesh (const MeshLayout* layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams, std::size_t meshSize) :
    m_layout{layout},
    m_indices{std::move(meshIndices)},
    m_streams{std::move(meshStreams)},
    m_size{meshSize} {
    PHENYL_DASSERT(m_layout);
}

void Mesh::bind (Pipeline& pipeline, const std::vector<BufferBinding>& streamBindings) const {
    PHENYL_DASSERT(m_streams.size() == streamBindings.size());
    for (std::size_t i = 0; i < m_streams.size(); i++) {
        pipeline.bindBuffer(streamBindings[i], m_streams[i]);
    }
    pipeline.bindIndexBuffer(layout().indexType, indices());
}

Meshes::Builder::Builder (Meshes& meshes, Renderer& renderer) : m_meshes{meshes}, m_renderer{renderer} {}

Meshes::Builder& Meshes::Builder::withStream (std::span<const std::byte> data, std::size_t stride) {
    auto& stream = m_streams.emplace_back(m_renderer.makeRawBuffer(stride, data.size(), BufferStorageHint::STATIC));
    m_streamStrides.emplace_back(stride);
    stream.upload(data);
    return *this;
}

Meshes::Builder& Meshes::Builder::withAttribute (MeshAttribute attribute) {
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

Meshes::Builder& Meshes::Builder::withIndices (ShaderIndexType indexType, std::span<const std::byte> data,
    std::size_t size) {
    PHENYL_ASSERT_MSG(!m_indexBuffer, "Indices already exist");
    PHENYL_DASSERT(size <= data.size() / GetIndexTypeSize(indexType));
    m_indexBuffer = m_renderer.makeRawBuffer(GetIndexTypeSize(indexType), data.size(), BufferStorageHint::STATIC, true);

    m_indexBuffer->upload(data);
    m_indexType = indexType;
    m_size = size;
    return *this;
}

Meshes::Builder& Meshes::Builder::withSize (std::size_t size) {
    m_size = m_size ? std::min(*m_size, size) : size;
    return *this;
}

std::shared_ptr<Mesh> Meshes::Builder::build () {
    // TODO
    PHENYL_ASSERT_MSG(m_indexBuffer, "Mesh must have index buffer (TODO)");
    PHENYL_ASSERT_MSG(m_size, "Mesh must have size");
    PHENYL_DASSERT(m_streamStrides.size() == m_streams.size());

    const auto& layout = m_meshes.findLayout({
      .indexType = *m_indexType,
      .attributes = std::move(m_attribs),
      .streamStrides = std::move(m_streamStrides),
    });
    return std::make_shared<Mesh>(&layout, std::move(*m_indexBuffer), std::move(m_streams), *m_size);
}

Meshes::Meshes (Renderer& renderer) : m_renderer{renderer} {}

Meshes::Builder Meshes::builder () {
    return Builder{*this, m_renderer};
}

std::shared_ptr<Mesh> Meshes::load (core::AssetLoadContext& ctx) {
    return ctx.read([&] (std::istream& data) {
        // Only obj files supported for now
        ObjFile objFile{data};
        auto mesh = objFile.makeMesh(*this);
        return mesh;
    });
}

void Meshes::selfRegister () {
    core::Assets::AddManager(this);
}

std::string_view Meshes::getName () const noexcept {
    return "Meshes";
}

const MeshLayout& Meshes::findLayout (MeshLayout&& layout) {
    if (auto it = m_layouts.find(layout); it != m_layouts.end()) {
        return *it;
    }

    layout.layoutId = m_nextLayoutId++;
    auto it = m_layouts.emplace(std::move(layout)).first;
    return *it;
}
