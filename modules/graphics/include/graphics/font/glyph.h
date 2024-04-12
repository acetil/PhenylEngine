#pragma once

#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    struct Glyph {
        glm::vec2 uvStart;
        glm::vec2 uvEnd;
        std::uint32_t atlasLayer;

        float advance;
        glm::ivec2 bearing;
        glm::vec2 size;
    };

    class IGlyphRenderer {
    public:
        virtual ~IGlyphRenderer () = default;

        virtual void renderGlyph (const Glyph& glyph, glm::vec2 pos, glm::vec3 colour) = 0;
    };
}