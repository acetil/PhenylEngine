#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/material.h"

namespace phenyl::graphics {
class MaterialManager : public core::AssetManager<Material> {
public:
    MaterialManager (Renderer& renderer);

    std::shared_ptr<Material> load (core::AssetLoadContext& ctx) override;

    void selfRegister ();

private:
    Renderer& m_renderer;
};
} // namespace phenyl::graphics
