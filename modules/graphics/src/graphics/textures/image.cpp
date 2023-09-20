#include <string>
#include <utility>
#include <vector>

#include "graphics/textures/image.h"
#include "stb/stb_image.h"
#include "logging/logging.h"
using namespace phenyl::graphics;

Image::Image (const char* filename, std::string name) {
    width = 0; // to get clang to shut up
    height = 0;
    n = 0;
    unsigned char* data = stbi_load(filename, &width, &height, &n, 4);
    if (data == nullptr) {
        logging::log(LEVEL_ERROR, "Failed to load texture \"{}\" at path \"{}\"", name, filename);
        return;
    }
    this->name = std::move(name);

    imageData = std::unique_ptr<std::byte[], DataDeleter>((std::byte*)data, DataDeleter{true});
}

std::byte* Image::getData () const {
    return imageData.get();
}
Image::~Image () = default;

int Image::getArea () const {
    return width * height;
}
int Image::getWidth () const {
    return width;
}
int Image::getHeight () const {
    return height;
}
std::string Image::getName () {
    return name;
}

Image::Image (Image&& other) noexcept : imageData{std::move(other.imageData)}, width{other.width}, height{other.height}, n{other.n}, name{std::move(other.name)} {
    other.width = 0;
    other.height = 0;
    other.n = 0;
}

Image& Image::operator= (Image&& other) noexcept {
    imageData = std::move(other.imageData);
    width = other.width;
    height = other.height;
    n = other.n;
    name = std::move(other.name);

    other.width = 0;
    other.height = 0;
    other.n = 0;

    return *this;
}

Image::Image (std::string name, int width, int height, int n) : name{std::move(name)}, width{width}, height{height}, n{n} {
    imageData = std::unique_ptr<std::byte[], DataDeleter>{new std::byte[width * height * n], DataDeleter{false}};
}

Image::Image (std::istream& file) {
    std::vector<unsigned char> contents{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    width = 0; // to get clang to shut up
    height = 0;
    n = 0;
    unsigned char* data = stbi_load_from_memory(contents.data(), static_cast<int>(contents.size()), &width, &height, &n, 4);
    if (data == nullptr) {
        logging::log(LEVEL_ERROR, "Failed to load texture from istream!");
        return;
    }

    imageData = std::unique_ptr<std::byte[], DataDeleter>((std::byte*)data, DataDeleter{true});
}

Image::operator bool () const {
    return (bool)imageData;
}

void Image::DataDeleter::operator() (std::byte* data) const noexcept {
    if (isSTB) {
        stbi_image_free(data);
    } else {
        delete[] data;
    }
}
