#include <string>
#include <stdlib.h>
#include "texture.h"

#define NUM_TRIANGLE_VERTICES 3
#define TRIANGLES_PER_TEX 2
#define NUM_UV_PER_VERTEX 2
std::string graphics::Texture::getName () {
    return name;
}

graphics::Texture::Texture () {
    this->texUvs = NULL;
}

float* graphics::Texture::getTexUvs () {
    return texUvs;
}

void graphics::Texture::setTexUvs (int uPixel, int vPixel, int xSize, int ySize, int atlasXSize, int atlasYSize) {
    if (texUvs != NULL) {
        delete[] texUvs;
    }
    //texUvs = (float*) malloc(sizeof(float) * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_TEX * NUM_UV_PER_VERTEX);
    texUvs = new float[NUM_TRIANGLE_VERTICES * TRIANGLES_PER_TEX * NUM_UV_PER_VERTEX];
    float* uvPtr = texUvs;
    for (int i = 0; i < NUM_TRIANGLE_VERTICES * TRIANGLES_PER_TEX; i++) {
        // see Graphics::drawTexSquare() in graphics.cpp
        int correctedVertex = i % NUM_TRIANGLE_VERTICES + i / NUM_TRIANGLE_VERTICES;
        float u = (float)uPixel / atlasXSize;
        float v = (float)vPixel / atlasYSize;

        if (correctedVertex >= 2) {
            v += (float)ySize / atlasYSize;
        }
        if (correctedVertex % 2 == 1) {
            u += (float)xSize / atlasXSize; 
        }
        *(uvPtr++) = u;
        *(uvPtr++) = v;
    }
}
graphics::Texture::~Texture () {
    if (texUvs != NULL) {
        delete[] texUvs;
    }
}
void graphics::Texture::setName (std::string name) {
    this->name = name;
}