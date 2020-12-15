#include "font.h"

#define NUM_VERTICES 6
#define NUM_POS_COMP 2

using namespace graphics;

void graphics::Font::renderText (const std::string& text, int size, int x, int y, Buffer* buffer) {
    float vertexPositions[NUM_VERTICES * NUM_POS_COMP];
    face.setFontSize(size);
    int last = 0;
    int currX = x;
    for (char c : text) {
        int curr = (int) ((unsigned int)c);
        float* posPtr = vertexPositions;
        auto off = face.renderText(curr, last);
        for (int i = 0; i < NUM_VERTICES; i++) {
            int correctedVertex = 3 * (i == 3) + i % 3;
            *(posPtr++) = (float) (currX + off.offsetX + (correctedVertex & 1) * off.width) / (float)screenX * 2.0f - 1.0f;
            *(posPtr++) = (float) (y + off.offsetY + ((correctedVertex & 2) >> 1) * off.height) / (float)screenY * -1.0f * 2.0f + 1.0f;
        }
        buffer[0].pushData(vertexPositions, NUM_VERTICES * NUM_POS_COMP);
        atlas.bufferChar(buffer[1], curr);
        currX += off.advance;
    }
}

void Font::loadAtlas (Renderer* renderer) {
    atlas.loadAtlas(renderer);
}

void Font::bindAtlasTexture () {
    atlas.getTex().bindTexture();
}
