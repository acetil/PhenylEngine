#pragma once

#include <vector>

#include "graphics/maths_headers.h"
#include "graphics/renderers/renderer.h"
#include "graphics/image.h"

#include "util/optional.h"

namespace phenyl::graphics {
    struct AtlasOffset {
        glm::vec2 topLeft;
        glm::vec2 bottomRight;
    };
    class TextureAtlas {
    private:
        ImageTexture texture;
        std::vector<AtlasOffset> items;
    public:
        explicit TextureAtlas (Renderer* renderer);

        void build (const std::vector<Image*>& images);

        [[nodiscard]] std::size_t size () const;
        const AtlasOffset& operator[] (std::size_t index) const;
        [[nodiscard]] util::Optional<const AtlasOffset&> at (std::size_t index) const;
        //void bind ();

        const Texture& getTexture () const;
    };
}