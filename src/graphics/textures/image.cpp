#include <string>
#include <utility>

#include "image.h"
#include "stb/stb_image.h"
#include "logging/logging.h"
using namespace graphics;

graphics::Image::Image (const char* filename, std::string name) {
    width = 0; // to get clang to shut up
    height = 0;
    n = 0;
    data = stbi_load(filename, &width, &height, &n, 4);
    if (data == nullptr) {
        logging::logf(LEVEL_ERROR, R"(Failed to load texture "%s" at path "%s")", name.c_str(), filename);
    }
    this->name = std::move(name);
}
unsigned char* graphics::Image::getData () {
    return data;
}
graphics::Image::~Image () {
    stbi_image_free(data);
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