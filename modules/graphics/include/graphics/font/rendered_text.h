#pragma once

#include <memory>

#include "graphics/maths_headers.h"
#include "util/span.h"

#define TEXT_GLYPH_SIZE 6

namespace phenyl::graphics {
    class RenderedText {
    private:
        std::unique_ptr<glm::vec2[]> posComp;
        std::unique_ptr<glm::vec2[]> uvComp;
        std::unique_ptr<glm::vec3[]> colourComp;
        std::size_t maxTextLen;
        std::size_t textLen = 0;
        glm::vec2 currentOffset = {0, 0};
        glm::vec2 topLeft = {0, 0};
        glm::vec2 bottomRight = {0, 0};
    public:
        explicit RenderedText (std::size_t textLen) : maxTextLen{textLen} {
            posComp = std::make_unique<glm::vec2[]>(textLen * TEXT_GLYPH_SIZE);
            uvComp = std::make_unique<glm::vec2[]>(textLen * TEXT_GLYPH_SIZE);
            colourComp = std::make_unique<glm::vec3[]>(textLen * TEXT_GLYPH_SIZE);
        }

        void addGlyph (glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 screenSize, util::span<glm::vec2> uvs, glm::vec3 colour);
        void finishRender (glm::vec2 screenSize);

        [[nodiscard]] util::span<glm::vec2> getPosComp () const;
        [[nodiscard]] util::span<glm::vec2> getUvComp () const;
        [[nodiscard]] util::span<glm::vec3> getColourComp () const;
        void setOffset (glm::vec2 offset, glm::vec2 screenSize);

        glm::vec2 getSize ();
    };
}