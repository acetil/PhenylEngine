#include <string>

#include "image.h"
#include "stb/stb_image.h"
using namespace graphics;

graphics::Image::Image (const char* filename, std::string name) {
    data = stbi_load(filename, &width, &height, &n, 4);
    this->name = name;
}
unsigned char* graphics::Image::getData () {
    return data;
}
graphics::Image::~Image () {
    stbi_image_free(data);
    data = NULL;
}
int graphics::Image::getArea () {
    return width * height;
}
int graphics::Image::getWidth () {
    return width;
}
int graphics::Image::getHeight () {
    return height;
}
std::string graphics::Image::getName () {
    return name;
}