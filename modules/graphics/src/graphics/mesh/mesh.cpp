#include "graphics/mesh/mesh.h"

using namespace phenyl::graphics;

Mesh::Mesh (MeshLayout layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams, std::size_t meshSize) : m_layout{std::move(layout)}, m_indices{std::move(meshIndices)}, m_streams{std::move(meshStreams)}, m_size{meshSize} {}
