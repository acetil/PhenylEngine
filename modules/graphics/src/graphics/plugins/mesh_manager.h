#pragma once

#include "core/assets/asset_manager.h"
#include "util/map.h"

#include "graphics/renderer.h"
#include "graphics/mesh/mesh.h"

namespace phenyl::graphics {
    class MeshManager : public core::AssetManager<Mesh> {
    private:
        Renderer& renderer;
        util::HashMap<std::size_t, std::unique_ptr<Mesh>> meshes;
        util::HashMap<MeshLayout, std::uint64_t> layoutIds;
        std::uint64_t nextLayoutId = 1;
    public:
        MeshManager (Renderer& renderer);

        void selfRegister ();

        Mesh* load (std::ifstream& data, std::size_t id) override;
        Mesh* load (Mesh&& obj, std::size_t id) override;
        const char* getFileType() const override;
        bool isBinary() const override;
        void queueUnload(std::size_t id) override;
    };
}
