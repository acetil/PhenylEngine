#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/renderer.h"
#include "graphics/mesh/mesh.h"

namespace phenyl::graphics {
class MeshManager : public core::AssetManager<Mesh> {
public:
    MeshManager (Renderer& renderer);

    void selfRegister ();

    Mesh* load (std::ifstream& data, std::size_t id) override;
    Mesh* load (Mesh&& obj, std::size_t id) override;
    const char* getFileType () const override;
    bool isBinary () const override;
    void queueUnload (std::size_t id) override;

private:
    Renderer& m_renderer;
    util::HashMap<std::size_t, std::unique_ptr<Mesh>> m_meshes;
    util::HashMap<MeshLayout, std::uint64_t> m_layoutIds;
    std::uint64_t m_nextLayoutId = 1;
};
} // namespace phenyl::graphics
