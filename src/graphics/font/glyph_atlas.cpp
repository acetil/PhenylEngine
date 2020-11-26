#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphics/maths_headers.h"
#include "glyph_atlas.h"
#include "logging/logging.h"

#define TARGET_MAX_RES 32

#define EM_SIZE 72

#define SAMPLE_THRESHOLD 0.5
#define DISTANCE_LIMIT 12

using namespace graphics;

struct GlyphDistanceField {
    int width = 0;
    int height = 0;
    unsigned char* data = nullptr;
    int glyphIndex = 0;
    GlyphDistanceField () = default;
    GlyphDistanceField (int _width, int _height, int _glyphIndex) : width(_width), height(_height), glyphIndex(_glyphIndex) {
        data = new unsigned char[width * height];
    }
    GlyphDistanceField(GlyphDistanceField&) = delete;
    GlyphDistanceField(GlyphDistanceField&& other) : width(other.width), height(other.height),
            data(std::exchange(other.data, nullptr)), glyphIndex(other.glyphIndex) {}
    ~GlyphDistanceField () {
        delete[] data;
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
    double lowDist = limit * sqrt(2);
    for (int i = - limit + 1; i < limit; i++) {
        for (int j = -limit + 1; j < limit; j++) {
            if (data[y * width + x] != data[(y + i) * width + x + j] && sqrt(i * i + j * j) < lowDist) {
                lowDist = sqrt(i * i + j * j);
            }
        }
    }
    lowDist *= mult;
    lowDist /= limit * sqrt(2) * 2;
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
        logging::log(LEVEL_DEBUG, "width = {}, height = {}, newWidth = {}, newHeight = {}", img.width, img.height,
                     newWidth, newHeight);
        auto isPixel = getReducedPixels(img, newWidth, newHeight);
        GlyphDistanceField glyphDist = getDistField(isPixel, newWidth, newHeight, img.glyphIndex);
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
        delete[] isPixel;
        distFieldGlyphs.emplace_back(std::move(glyphDist));
    }
    /*width = distFieldGlyphs[0].width;
    height = distFieldGlyphs[0].height;
    data = new unsigned char[width * height];
    memcpy(data, distFieldGlyphs[0].data, width * height);*/
    width = 128;
    height = 128;
    data = new unsigned char[width * height];
    for (int i = 0; i < distFieldGlyphs[0].height; i++) {
        for (int j = 0; j < distFieldGlyphs[0].width; j++) {
            data[i * height + j] = distFieldGlyphs[0].data[distFieldGlyphs[0].width * i + j];
        }
    }
}
GlyphAtlas::~GlyphAtlas () {
    delete[] data;
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
