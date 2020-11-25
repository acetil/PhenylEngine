#include <stdio.h>
#include <math.h>
#include "graphics/maths_headers.h"
#include "glyph_atlas.h"
#include "logging/logging.h"

#define DISTANCE_FIELD_RES 11

#define TARGET_MAX_RES 32

#define EM_SIZE 72

#define SAMPLE_THRESHOLD 0.5

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

graphics::GlyphAtlas::GlyphAtlas (const std::vector<GlyphImage>& glyphs, int targetRes) {
    // Uses valve distance field technique
    // See https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
    std::vector<GlyphDistanceField> distFieldGlyphs;
    for (const auto& img : glyphs) {
        // since emX = targetRes and oldEmX = pointSize * xRes / 72,
        // newWidth = width * emX / oldEmX = width * targetRes * 72 / (pointSize * xRes)
        int newWidth = (int)floor(img.width * targetRes * EM_SIZE / (double)(img.fontSize * img.xRes));
        // since emY = targetRes * yRes / xRes and oldEmY = pointSize * yRes / 72
        // newHeight = height * emY / oldEmY = height * targetRes * yRes / xRes * 72 / (pointSize * yRes)
        //           = height * targetRes * 72 / (pointSize * yRes)
        int newHeight = (int)floor(img.height * targetRes * EM_SIZE / (double)(img.fontSize * img.xRes));
        logging::log(LEVEL_DEBUG, "width = {}, height = {}, newWidth = {}, newHeight = {}", img.width, img.height, newWidth, newHeight);
        GlyphDistanceField glyphDist(newWidth, newHeight, img.glyphIndex);
        auto isPixel = getReducedPixels(img, newWidth, newHeight);
        /*for (int i = 0; i < img.height; i++) {
            for (int j = 0; j < img.width; j++) {
                printf("%-3d ", img.data[i * img.width + j]);
            }
            printf("\n");
        }
        printf("====\n");*/
        for (int i = 0; i < newHeight; i++) {
            for (int j = 0; j < newWidth; j++) {
                printf("%c", isPixel[i * newWidth + j] ? '*' : ' ');
            }
            printf("\n");
        }
    }
}

GlyphAtlas::~GlyphAtlas () {
    delete[] data;
}
