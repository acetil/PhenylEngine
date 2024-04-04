#pragma once

#include <unordered_map>

#include "graphics/image.h"
#include "util/map.h"
#include "graphics/renderers/texture.h"
#include "graphics/renderers/renderer.h"

namespace phenyl::graphics {
    struct ColumnAtlas {
        struct Column {
            std::uint32_t offset;
            std::uint32_t width;
            std::uint32_t currHeight;
        };

        Image atlasImage;
        std::uint32_t index;
        std::vector<Column> columns;
        std::uint32_t currWidth = 0;
        std::uint32_t padding;
        bool needsUpload = false;

        explicit ColumnAtlas (std::uint32_t index, std::uint32_t size, std::uint32_t padding = 1);

        std::optional<glm::uvec2> place (const Image& image);
    };

    class GlyphAtlas {
    public:
        struct Placement {
            glm::vec2 uvStart;
            glm::vec2 uvEnd;
            std::uint32_t atlasLayer;
        };
    private:
        ImageArrayTexture arrayTexture;
        std::vector<ColumnAtlas> atlases;
        std::uint32_t size;
        std::uint32_t padding;

        glm::vec3 whitePixel;
    public:
        explicit GlyphAtlas (Renderer& renderer, std::uint32_t size = 1024, std::uint32_t padding=1);
        Placement place (const Image& image);

        [[nodiscard]] const ISampler& sampler () const;
        void upload ();

        [[nodiscard]] glm::vec3 opaque () const {
            return whitePixel;
        }
    };
}

