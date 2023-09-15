#include "graphics/font/font.h"

#include "logging/logging.h"

#define NUM_VERTICES 6
#define NUM_POS_COMP 2
#define NUM_COLOUR_COMP 3

using namespace graphics;

/*void graphics::Font::renderText (const std::string& text, int size, int x, int y, glm::vec3 colour, Buffer* buffer) {
    float vertexPositions[NUM_VERTICES * NUM_POS_COMP];
    float vertexColour[NUM_VERTICES * NUM_COLOUR_COMP];
    face.setFontSize(size);

    //logging::log(LEVEL_DEBUG, "Text colour: {} {} {}", colour.r, colour.g, colour.b);

    int last = 0;
    int currX = x;
    for (char c : text) {
        int curr = (int) ((unsigned int)c);
        float* posPtr = vertexPositions;
        float* colourPtr = vertexColour;
        auto off = face.renderText(curr, last);
        for (int i = 0; i < NUM_VERTICES; i++) {
            int correctedVertex = 3 * (i == 3) + i % 3;
            *(posPtr++) = (float) (currX + off.offsetX + (correctedVertex & 1) * off.width) / (float)screenX * 2.0f - 1.0f;
            *(posPtr++) = (float) (y + off.offsetY + ((correctedVertex & 2) >> 1) * off.height) / (float)screenY * -1.0f * 2.0f + 1.0f;
            *(colourPtr++) = colour.r;
            *(colourPtr++) = colour.g;
            *(colourPtr++) = colour.b;
        }
        buffer[0].pushData(vertexPositions, NUM_VERTICES * NUM_POS_COMP);
        atlas.bufferChar(buffer[1], curr);
        buffer[2].pushData(vertexColour, NUM_VERTICES * NUM_COLOUR_COMP);
        currX += off.advance;
        last = curr;
    }
}*/

void Font::loadAtlas (Renderer* renderer) {
    atlas.loadAtlas(renderer);
}

void Font::bindAtlasTexture () {
    atlas.getTex().bindTexture();
}

GraphicsTexture& Font::getAtlasTexture () {
    // TODO: it now moves, fix lifetime issues
    return atlas.getTex();
}

RenderedText Font::renderText (const std::string& text, int size, int x, int y, glm::vec3 colour) {
    RenderedText renderedText{text.size()};
    face.setFontSize(size);

    int last = 0;
    int currX = x;
    for (char c : text) {
        int curr = (int)((unsigned int) c);
        auto off = face.renderText(curr, last);

        renderedText.addGlyph({(currX + off.offsetX), y + off.offsetY}, {off.width, off.height}, {screenX, screenY}, atlas.getCharUvs(c), colour);
        currX += off.advance;
        last = curr;
    }
    renderedText.finishRender({screenX, screenY});
    return renderedText;
}
