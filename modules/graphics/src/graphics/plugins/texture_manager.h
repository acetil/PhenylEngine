#pragma once

#include "core/assets/asset_manager.h"
#include "graphics/backend/texture.h"

namespace phenyl::graphics {
class Renderer;

class TextureManager : public core::AssetManager<Texture> {
public:
    TextureManager (Renderer& renderer);

    Texture* load (std::ifstream& data, std::size_t id) override;
    std::shared_ptr<Texture> load2 (std::ifstream& data) override;
    Texture* load (Texture&& obj, std::size_t id) override;

    const char* getFileType () const override;
    bool isBinary () const override;
    void queueUnload (std::size_t id) override;

    void selfRegister ();

private:
    Renderer& m_renderer;
    std::unordered_map<std::size_t, std::unique_ptr<Texture>> m_textures;
};
} // namespace phenyl::graphics
