#include "graphics/font/rendered_text.h"
#include "logging/logging.h"

using namespace graphics;

void RenderedText::addGlyph (glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 screenSize, util::span<glm::vec2> uvs, glm::vec3 colour) {
    if (textSize == maxTextSize) {
        logging::log(LEVEL_ERROR, "Too many glyphs for rendered text!");
        return;
    }
    glm::vec2* posPtr = posComp.get() + textSize * TEXT_GLYPH_SIZE;
    glm::vec3* colourPtr = colourComp.get() + textSize * TEXT_GLYPH_SIZE;
    glm::vec2* uvPtr = uvComp.get() + textSize * TEXT_GLYPH_SIZE;
    for (int i = 0; i < TEXT_GLYPH_SIZE; i++) {
        int compNum = i == 3 ? 3 : i % 3;

        glm::vec2 pos = topLeftPos;
        if (compNum % 2 == 1) {
            pos.x += size.x;
        }
        if (compNum / 2 == 1) {
            pos.y += size.y;
        }

        pos = pos / screenSize * 2.0f - glm::vec2{1.0f, 1.0f};
        pos.y *= -1.0f;

        *(posPtr++) = pos;
        *(colourPtr++) = colour;
        *(uvPtr++) = uvs.begin()[i];
    }

    textSize++;
}

util::span<glm::vec2> RenderedText::getPosComp () const {
    return {posComp.get(), textSize * TEXT_GLYPH_SIZE};
}

util::span<glm::vec2> RenderedText::getUvComp () const {
    return {uvComp.get(), textSize * TEXT_GLYPH_SIZE};
}

util::span<glm::vec3> RenderedText::getColourComp () const {
    return {colourComp.get(), textSize * TEXT_GLYPH_SIZE};
}
