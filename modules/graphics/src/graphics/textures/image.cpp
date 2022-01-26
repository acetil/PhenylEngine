#include <string>
#include <utility>

#include "graphics/textures/image.h"
#include "stb/stb_image.h"
#include "logging/logging.h"
using namespace graphics;

graphics::Image::Image (const char* filename, std::string name) {
    width = 0; // to get clang to shut up
    height = 0;
    n = 0;
    data = stbi_load(filename, &width, &height, &n, 4);
    if (data == nullptr) {
        logging::log(LEVEL_ERROR, "Failed to load texture \"{}\" at path \"{}\"", name, filename);
    }
    this->name = std::move(name);
    isSTB = true;
}
graphics::Image::Image (unsigned char* data, int width, int height, bool monochrome, const std::string& name) {
    this->width = width;
    this->height = height;
    this->name = name;
    this->n = 4;
    isSTB = false;
    if (!monochrome) {
        this->data = data;
    } else {
        this->data = new unsigned char[width * height * 4];
        for (int i = 0; i < width * height * 4; i++) {
            if (i % 4 == 3) {
                this->data[i] = 255;
            } else {
                this->data[i] = data[i % 3];
            }
        }
        free(data);
    }
}
unsigned char* graphics::Image::getData () {
    return data;
}
graphics::Image::~Image () {
    if (isSTB) {
        stbi_image_free(data);
    } else {
        delete[] data;
    }
    data = nullptr;
}
int graphics::Image::getArea () const {
    return width * height;
}
int graphics::Image::getWidth () const {
    return width;
}
int graphics::Image::getHeight () const {
    return height;
}
std::string graphics::Image::getName () {
    return name;
}