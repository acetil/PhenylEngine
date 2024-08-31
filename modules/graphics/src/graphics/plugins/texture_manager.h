#pragma once

#include "graphics/texture.h"
#include "common/assets/asset_manager.h"

namespace phenyl::graphics {
    class Renderer;

    class TextureManager : public common::AssetManager<Texture> {
    private:
        Renderer& renderer;
        std::unordered_map<std::size_t, std::unique_ptr<Texture>> textures;
    public:
        TextureManager (Renderer& renderer);

        Texture* load (std::ifstream& data, std::size_t id) override;
        Texture* load (Texture&& obj, std::size_t id) override;

        const char* getFileType () const override;
        bool isBinary () const override;
        void queueUnload (std::size_t id) override;

        void selfRegister ();
    };
}
