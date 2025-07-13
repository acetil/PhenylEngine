#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/material.h"

namespace phenyl::graphics {
class MaterialInstanceManager : public core::AssetManager<MaterialInstance> {
public:
    std::shared_ptr<MaterialInstance> load (core::AssetLoadContext& ctx) override;
    void selfRegister ();
};
} // namespace phenyl::graphics
