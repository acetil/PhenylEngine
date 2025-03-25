#include "graphics/mesh/mesh.h"

using namespace phenyl::graphics;

Mesh::Mesh (MeshLayout layout, RawBuffer meshIndices, std::vector<RawBuffer> meshStreams, std::size_t meshSize) : meshLayout{std::move(layout)}, meshIndices{std::move(meshIndices)}, meshStreams{std::move(meshStreams)}, meshSize{meshSize} {}
