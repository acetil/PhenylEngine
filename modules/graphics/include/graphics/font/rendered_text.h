#pragma once

#include <memory>

#include "graphics/maths_headers.h"
#include "util/span.h"

#define TEXT_GLYPH_SIZE 6

namespace graphics {
    class RenderedText {
    private:
        std::unique_ptr<glm::vec2[]> posComp;
        std::unique_ptr<glm::vec2[]> uvComp;
        std::unique_ptr<glm::vec3[]> colourComp;
        std::size_t maxTextSize;
        std::size_t textSize = 0;
    public:
        explicit RenderedText (std::size_t textSize) : maxTextSize{textSize} {
            posComp = std::make_unique<glm::vec2[]>(textSize * TEXT_GLYPH_SIZE);
            uvComp = std::make_unique<glm::vec2[]>(textSize * TEXT_GLYPH_SIZE);
            colourComp = std::make_unique<glm::vec3[]>(textSize * TEXT_GLYPH_SIZE);
        }

        void addGlyph (glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 screenSize, util::span<glm::vec2> uvs, glm::vec3 colour);

        [[nodiscard]] util::span<glm::vec2> getPosComp () const;
        [[nodiscard]] util::span<glm::vec2> getUvComp () const;
        [[nodiscard]] util::span<glm::vec3> getColourComp () const;
    };
}