#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/material.h"

namespace phenyl::graphics {
    class MaterialManager : public core::AssetManager<Material> {
    private:
        Renderer& renderer;
        std::unordered_map<std::size_t, std::unique_ptr<Material>> materials;
    public:
        MaterialManager (Renderer& renderer);

        Material* load (std::ifstream& data, std::size_t id) override;
        Material* load (Material&& obj, std::size_t id) override;
        const char* getFileType() const override;
        void queueUnload(std::size_t id) override;

        void selfRegister ();
    };
}
