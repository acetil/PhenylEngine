#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/texture.h"

namespace phenyl::graphics {
class Renderer;

class TextureManager : public core::AssetManager<Texture> {
public:
    TextureManager (Renderer& renderer);

    std::shared_ptr<Texture> load (std::ifstream& data) override;

    const char* getFileType () const override;
    bool isBinary () const override;

    void selfRegister ();

private:
    Renderer& m_renderer;
};
} // namespace phenyl::graphics
