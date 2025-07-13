#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/renderer.h"
#include "graphics/mesh/mesh.h"

namespace phenyl::graphics {
class MeshManager : public core::AssetManager<Mesh> {
public:
    MeshManager (Renderer& renderer);

    void selfRegister ();
    std::shared_ptr<Mesh> load (core::AssetLoadContext& ctx) override;

private:
    Renderer& m_renderer;
    util::HashMap<MeshLayout, std::uint64_t> m_layoutIds;
    std::uint64_t m_nextLayoutId = 1;
};
} // namespace phenyl::graphics
