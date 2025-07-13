#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/texture.h"

namespace phenyl::graphics {
class Renderer;

class TextureManager : public core::AssetManager<Texture> {
public:
    TextureManager (Renderer& renderer);

    std::shared_ptr<Texture> load (core::AssetLoadContext& ctx) override;
    void selfRegister ();

private:
    Renderer& m_renderer;
};
} // namespace phenyl::graphics
