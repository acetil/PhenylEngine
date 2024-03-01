#include "graphics/font/rendered_text.h"
#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"RENDERED_TEXT"};

static glm::vec2 screenToWindowSpace (glm::vec2 vec, glm::vec2 screenSize) {
    glm::vec2 pos = vec / screenSize * 2.0f - glm::vec2{1.0f, 1.0f};
    pos.y *= -1.0f;

    return pos;
}

static glm::vec2 windowToScreenSpace (glm::vec2 pos, glm::vec2 screenSize) {
    glm::vec2 vec = pos;
    vec.y *= -1.0f;
    vec += glm::vec2{1.0f, 1.0f};
    vec /= 2.0f;
    vec *= screenSize;

    return vec;
}

void RenderedText::addGlyph (glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 screenSize, util::span<glm::vec2> uvs, glm::vec3 colour) {
    if (textLen == maxTextLen) {
        PHENYL_LOGE(LOGGER, "Too many glyphs for rendered text (max={})!", maxTextLen);
        return;
    }
    glm::vec2* posPtr = posComp.get() + textLen * TEXT_GLYPH_SIZE;
    glm::vec3* colourPtr = colourComp.get() + textLen * TEXT_GLYPH_SIZE;
    glm::vec2* uvPtr = uvComp.get() + textLen * TEXT_GLYPH_SIZE;

    if (topLeftPos.x < topLeft.x) {
        topLeft.x = topLeftPos.x;
    }
    if (topLeftPos.y < topLeft.y) {
        topLeft.y = topLeftPos.y;
    }
    if ((topLeftPos + size).x > bottomRight.x) {
        bottomRight.x = (topLeftPos + size).x;
    }
    if ((topLeftPos + size).y > bottomRight.y) {
        bottomRight.y = (topLeftPos + size).y;
    }

    for (int i = 0; i < TEXT_GLYPH_SIZE; i++) {
        int compNum = i == 3 ? 3 : i % 3;

        glm::vec2 pos = topLeftPos;
        if (compNum % 2 == 1) {
            pos.x += size.x;
        }
        if (compNum / 2 == 1) {
            pos.y += size.y;
        }

        //pos = pos / screenSize * 2.0f - glm::vec2{1.0f, 1.0f};
        //pos.y *= -1.0f;
        pos = screenToWindowSpace(pos, screenSize);

        *(posPtr++) = pos;
        *(colourPtr++) = colour;
        *(uvPtr++) = uvs.begin()[i];
    }

    textLen++;
}

phenyl::util::span<glm::vec2> RenderedText::getPosComp () const {
    return {posComp.get(), textLen * TEXT_GLYPH_SIZE};
}

phenyl::util::span<glm::vec2> RenderedText::getUvComp () const {
    return {uvComp.get(), textLen * TEXT_GLYPH_SIZE};
}

phenyl::util::span<glm::vec3> RenderedText::getColourComp () const {
    return {colourComp.get(), textLen * TEXT_GLYPH_SIZE};
}

void RenderedText::setOffset (glm::vec2 offset, glm::vec2 screenSize) {
    glm::vec2 deltaOff = offset - currentOffset;
    if (deltaOff == glm::vec2{0.0f, 0.0f}) {
        return;
    }

    for (auto i = 0; i < textLen * TEXT_GLYPH_SIZE; i++) {
        glm::vec2 pos = windowToScreenSpace(posComp[i], screenSize);
        /*pos.y *= -1.0f;
        pos += glm::vec2{1.0f, 1.0f};
        pos /= 2.0f;
        pos *= screenSize;*/

        pos += deltaOff;
        //pos = pos / screenSize * 2.0f - glm::vec2{1.0f, 1.0f};
        //pos.y *= -1.0f;
        posComp[i] = screenToWindowSpace(pos, screenSize);
    }
    currentOffset = offset;
}

glm::vec2 RenderedText::getSize () {
    return bottomRight - topLeft;
}

void RenderedText::finishRender (glm::vec2 screenSize) {
    for (int i = 0; i < textLen * TEXT_GLYPH_SIZE; i++) {
        glm::vec2 pos = windowToScreenSpace(posComp[i], screenSize);
        pos -= topLeft;
        posComp[i] = screenToWindowSpace(pos, screenSize);
    }
    bottomRight -= topLeft;
    topLeft = {0, 0};
}
