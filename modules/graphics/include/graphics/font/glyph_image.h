#pragma once

#include <utility>

namespace graphics {
    class GlyphImage {
    public:
        unsigned char* data = nullptr;
        int width = 0;
        int height = 0;
        int glyphIndex = -1;
        int fontSize = 0;
        float xRes = 0.0f;
        float yRes = 0.0f;
        GlyphImage () = default;
        GlyphImage (unsigned char* _data, int _width, int _height, int _glyphIndex, int _fontSize, float _xRes, float _yRes) : data(_data), width(_width),
            height(_height), glyphIndex(_glyphIndex), fontSize(_fontSize), xRes(_xRes), yRes(_yRes) {}

        GlyphImage (GlyphImage& img) = delete;

        GlyphImage (GlyphImage&& img)  noexcept : data(std::exchange(img.data, nullptr)), width(img.width),
            height(img.height), glyphIndex(img.glyphIndex), fontSize(img.fontSize), xRes(img.xRes), yRes(img.yRes) {}

        ~GlyphImage () {
            delete[] data;
        }
    };
}
