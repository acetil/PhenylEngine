#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/material.h"

namespace phenyl::graphics {
    class MaterialInstanceManager : public core::AssetManager<MaterialInstance> {
    private:
        std::unordered_map<std::size_t, std::shared_ptr<MaterialInstance>> m_instances;

    public:
        MaterialInstance* load(std::ifstream& data, std::size_t id) override;
        MaterialInstance* load(MaterialInstance&& obj, std::size_t id) override;
        const char* getFileType() const override;
        void queueUnload(std::size_t id) override;

        void selfRegister ();
    };
}
