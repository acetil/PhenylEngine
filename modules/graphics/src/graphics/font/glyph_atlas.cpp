#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics/maths_headers.h"
#include "graphics/font/glyph_atlas.h"
#include "logging/logging.h"
#include "graphics/textures/build_atlas.h"
#define TARGET_MAX_RES 32

#define EM_SIZE 72

#define SAMPLE_THRESHOLD 0.5
#define DISTANCE_LIMIT 12

#define NUM_VERTICES 6
#define UV_PER_VERTEX 2

using namespace graphics;

struct GlyphDistanceField {
    int width = 0;
    int height = 0;
    unsigned char* data = nullptr;
    int glyphIndex = 0;
    int key = 0;
    GlyphDistanceField () = default;
    GlyphDistanceField (int _width, int _height, int _glyphIndex) : width(_width), height(_height), glyphIndex(_glyphIndex) {
        data = new unsigned char[width * height];
    }
    GlyphDistanceField(GlyphDistanceField&) = delete;
    GlyphDistanceField(GlyphDistanceField&& other)  noexcept : width(other.width), height(other.height),
            data(std::exchange(other.data, nullptr)), glyphIndex(other.glyphIndex), key(other.key) {}
    ~GlyphDistanceField () {
        delete[] data;
    }
    int getKey () const {
        return key;
    }
    int getXSize () const {
        return width;
    }
    int getYSize () const {
        return height;
    }
};

static unsigned char getSample (const unsigned char* data, int imgX, int imgY, int sampleX, int sampleY, int imgWidth, int imgHeight) {
    unsigned int totalValue = 0;
    int n = 0;
    for (int i = 0; i < 2 * sampleX + 1; i++) {
        for (int j = 0; j < 2 * sampleY + 1; j++) {
            int x = (i - sampleX) + imgX;
            int y = (i - sampleY) + imgY;
            if (x < 0 || y < 0 || x >= imgWidth || y >= imgHeight) {
                continue;
            }
            totalValue += data[y * imgWidth + x];
            n++;
        }
    }
    if (totalValue / (double)n / 255 >= SAMPLE_THRESHOLD) {
        return 255;
    } else {
        return 0;
    }
}

static unsigned char* getReducedPixels (const GlyphImage& img, int newWidth, int newHeight) {
    auto data = new unsigned char[newWidth * newHeight];
    int sampleX = (int)ceil(img.width / (double)newWidth);
    int sampleY = (int)ceil(img.width / (double)newWidth);
    for (int x = 0; x < newWidth; x++) {
        for (int y = 0; y < newHeight; y++) {
            int imgX = ceil(x * img.width / (double)newWidth);
            int imgY = ceil(y * img.height / (double)newHeight);
            data[y * newWidth + x] = getSample(img.data, imgX, imgY, sampleX, sampleY, img.width, img.height);
        }
    }
    return data;
}


static unsigned char signedDistance (unsigned char* data, int width, int height, int x, int y, int limit) {
    int mult = data[y * width + x] ? 1 : -1;
    double lowDist = 2 * limit * limit;
    for (int i = - limit + 1; i < limit; i++) {
        for (int j = -limit + 1; j < limit; j++) {
            if (data[y * width + x] != data[(y + i) * width + x + j] && i * i + j * j < lowDist) {
                lowDist = i * i + j * j;
            }
        }
    }
    lowDist /= limit * limit * 8;
    lowDist = sqrt(lowDist);
    lowDist *= mult;
    lowDist += 0.5;
    //printf("%f\n", lowDist);
    return (unsigned char) round(lowDist * 255);
}

static GlyphDistanceField getDistField (unsigned char* data, int newWidth, int newHeight, int glyphIndex) {
    GlyphDistanceField glyphDist(newWidth, newHeight, glyphIndex);
    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            glyphDist.data[y * newWidth + x] = signedDistance(data, newWidth, newHeight, x, y, DISTANCE_LIMIT);
        }
    }

    return glyphDist;
}

graphics::GlyphAtlas::GlyphAtlas (const std::vector<GlyphImage>& glyphs, int targetRes) {
    // Uses valve distance field technique
    // See https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
    std::vector<GlyphDistanceField> distFieldGlyphs;
    for (const auto& img : glyphs) {
        // since emX = targetRes and oldEmX = pointSize * xRes / 72,
        // newWidth = width * emX / oldEmX = width * targetRes * 72 / (pointSize * xRes)
        int newWidth = (int) floor(img.width * targetRes * EM_SIZE / (double) (img.fontSize * img.xRes));
        // since emY = targetRes * yRes / xRes and oldEmY = pointSize * yRes / 72
        // newHeight = height * emY / oldEmY = height * targetRes * yRes / xRes * 72 / (pointSize * yRes)
        //           = height * targetRes * 72 / (pointSize * yRes)
        int newHeight = (int) floor(img.height * targetRes * EM_SIZE / (double) (img.fontSize * img.xRes));
        //logging::log(LEVEL_DEBUG, "width = {}, height = {}, newWidth = {}, newHeight = {}", img.width, img.height,
                     //newWidth, newHeight);
        //auto isPixel = getReducedPixels(img, newWidth, newHeight);
        //GlyphDistanceField glyphDist = getDistField(isPixel, newWidth, newHeight, img.glyphIndex);
        GlyphDistanceField glyphDist = GlyphDistanceField(img.width, img.height, img.glyphIndex);
        memcpy(glyphDist.data, img.data, img.width * img.height);
        glyphDist.key = distFieldGlyphs.size();
        //GlyphDistanceField glyphDist(newWidth, newHeight, img.glyphIndex);
        //memcpy(glyphDist.data, isPixel, newWidth * newHeight);
        /*for (int i = 0; i < img.height; i++) {
            for (int j = 0; j < img.width; j++) {
                printf("%-3d ", img.data[i * img.width + j]);
            }
            printf("\n");
        }
        printf("====\n");*/
        /*for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                printf("%c", isPixel[i * newWidth + j] ? '*' : ' ');
            }
            printf("\n");
        }
        printf("====\n");*/
        //delete[] isPixel;
        distFieldGlyphs.emplace_back(std::move(glyphDist));
    }
    /*width = distFieldGlyphs[0].width;
    height = distFieldGlyphs[0].height;
    data = new unsigned char[width * height];
    memcpy(data, distFieldGlyphs[0].data, width * height);*/
    std::vector<AtlasObject<int>> vec;
    vec.reserve(distFieldGlyphs.size());
    int len = buildAtlas(distFieldGlyphs.begin(), distFieldGlyphs.end(), std::back_inserter(vec), 1);
    width = len;
    height = len;
    data = new unsigned char[width * height];
    memset(data, 0, width * height);
    uvs = new float[distFieldGlyphs.size() * NUM_VERTICES * UV_PER_VERTEX];
    float* uvPtr = uvs;
    //logging::log(LEVEL_DEBUG, "Vec size: {}", vec.size());
    for (auto obj : vec) {
        auto copyPtr = data + obj.xOff + obj.yOff * width;
        auto dataPtr = distFieldGlyphs[obj.key].data;
        for (int i = 0; i < distFieldGlyphs[obj.key].height; i++) {
            for (int j = 0; j < distFieldGlyphs[obj.key].width; j++) {
                copyPtr[j] = *(dataPtr++);
            }
            copyPtr += width;
        }
        float* originalPtr = uvPtr;
        for (int i = 0; i < NUM_VERTICES; i++) {
            unsigned int correctedVertex = 3 * (i == 3) + i % 3;
            *(uvPtr++) = obj.uOff + (float) (correctedVertex & 1) * obj.uSize;
            *(uvPtr++) = obj.vOff + (float) ((correctedVertex & 2) >> 1) * obj.vSize;
        }
        charUvs[distFieldGlyphs[obj.key].glyphIndex] = util::span(originalPtr, uvPtr);
    }
    /*logging::log(LEVEL_DEBUG, "Character keys: ");
    for (auto i : charUvs) {
        printf("%d\n", i.first);
    }*/
}
GlyphAtlas::~GlyphAtlas () {
    delete[] data;
    delete[] uvs;
}

void GlyphAtlas::loadAtlas (Renderer* renderer) {
    texture = renderer->loadTextureGrey(width, height, data);
    /*for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%c", data[i * width + j] ? '*' : ' ');
        }
        printf("\n");
    }*/
}

util::span<glm::vec2> GlyphAtlas::getCharUvs (int c) {
    if (!charUvs.contains(c)) {
        logging::log(LEVEL_DEBUG, "No glyph found with code {}", c);
        return {(glm::vec2*)charUvs[0].begin(), charUvs[0].size() / 2};
    } else {
        return {(glm::vec2*)charUvs[c].begin(), charUvs[c].size() / 2};
    }
}
