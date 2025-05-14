#include "graphics/image.h"

#include "graphics/detail/loggers.h"
#include "logging/logging.h"
#include "stb/stb_image.h"

#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"IMAGE", detail::GRAPHICS_LOGGER};

unsigned int Image::FormatComps (ImageFormat format) {
    switch (format) {
    case ImageFormat::R:
        return 1;
    case ImageFormat::RGBA:
        return 4;
    case ImageFormat::RGBA32:
        return 4;
    case ImageFormat::DEPTH24_STENCIL8:
        return 4;
    case ImageFormat::DEPTH:
        return 4;
    }

    PHENYL_ABORT("Invalid format: {}", static_cast<std::uint32_t>(format));
}

std::size_t Image::FormatSize (ImageFormat format) {
    switch (format) {
    case ImageFormat::R:
        return 1;
    case ImageFormat::RGBA:
        return 4;
    case ImageFormat::RGBA32:
        return 16;
    case ImageFormat::DEPTH24_STENCIL8:
        return 4;
    case ImageFormat::DEPTH:
        return 4;
    }

    PHENYL_ABORT("Invalid format: {}", static_cast<std::uint32_t>(format));
}

std::optional<Image> Image::Load (std::istream& file, ImageFormat format) {
    std::vector<unsigned char> contents{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    int width;
    int height;
    int n;

    auto* baseData = stbi_load_from_memory(contents.data(), static_cast<int>(contents.size()), &width, &height, &n,
        static_cast<int>(FormatComps(format)));
    if (!baseData) {
        PHENYL_LOGE(LOGGER, "Failed to load image from istream!");
        return std::nullopt;
    }

    DataPtr data{reinterpret_cast<std::byte*>(baseData), [] (std::byte* data) {
                     stbi_image_free(reinterpret_cast<void*>(data));
                 }};

    return Image{std::move(data), static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), format};
}

Image::Image (std::uint32_t width, std::uint32_t height, ImageFormat format) :
    Image{MakeData<float>(width * height * FormatComps(format)), width, height, format} {}

Image::Image (Image::DataPtr data, std::uint32_t width, std::uint32_t height, ImageFormat format) :
    m_data{std::move(data)},
    m_width{width},
    m_height{height},
    m_format{format} {}

Image Image::MakeNonOwning (std::span<std::byte> data, std::uint32_t width, std::uint32_t height, ImageFormat format) {
    PHENYL_ASSERT_MSG(data.size() == width * height * FormatSize(format), "Invalid data size: expected {}, got {}",
        width * height * FormatSize(format), data.size());

    return Image{DataPtr{data.data(),
                   [] (auto*) {
                   }},
      width, height, format};
}

void Image::blit (const Image& src, glm::uvec2 offset) {
    PHENYL_ASSERT(offset.x + src.width() <= width());
    PHENYL_ASSERT(offset.y + src.height() <= height());
    PHENYL_ASSERT_MSG(format() == src.format(), "Format mismatch: format conversion not yet supported!");

    auto formatSize = FormatSize(format());
    const auto* srcData = src.data().data();

    for (uint32_t y = 0; y < src.height(); y++) {
        std::memcpy(m_data.get() + ((y + offset.y) * width() + offset.x) * formatSize,
            srcData + y * src.width() * formatSize, src.width() * formatSize);
    }
}
