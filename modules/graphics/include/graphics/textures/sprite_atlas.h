#pragma once

#include "util/map.h"

#include "common/assets/asset_manager.h"
#include "texture_atlas.h"

namespace graphics {
    class SpriteTexture {
    private:
        std::unique_ptr<Image> image;
        std::size_t atlasIndex;
        AtlasOffset offset;
        friend class SpriteAtlas;
    public:
        explicit SpriteTexture (std::unique_ptr<Image> image) : image{std::move(image)}, atlasIndex{static_cast<std::size_t>(-1)}, offset{} {}
        [[nodiscard]] inline glm::vec2 topLeft () const {
            return offset.topLeft;
        }

        [[nodiscard]] inline glm::vec2 bottomRight () const {
            return offset.bottomRight;
        }
    };

    class SpriteAtlas : protected common::AssetManager<SpriteTexture> {
    private:
        TextureAtlas atlas;
        util::Map<std::size_t, std::unique_ptr<SpriteTexture>> sprites;
        std::vector<std::size_t> unloadQueue;
        bool rebuildRequired = false;


    protected:
        SpriteTexture* load (std::istream &data, std::size_t id) override;
        [[nodiscard]] const char* getFileType () const override;
        void queueUnload (std::size_t id) override;
        bool isBinary() const override;
    public:
        explicit SpriteAtlas (Renderer* renderer);

        bool rebuild ();
        void bind ();
    };
}