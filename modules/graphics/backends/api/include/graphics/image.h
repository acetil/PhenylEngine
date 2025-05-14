#pragma once

#include "graphics/maths_headers.h"

#include <memory>
#include <string>

namespace phenyl::graphics {
enum class ImageFormat {
    R,
    RGBA,
    RGBA32,
    DEPTH24_STENCIL8,
    DEPTH
};

class Image {
public:
    static std::uint32_t FormatComps (ImageFormat format);
    static std::size_t FormatSize (ImageFormat format);

    static std::optional<Image> Load (std::istream& file, ImageFormat format = ImageFormat::RGBA);
    static Image MakeNonOwning (std::span<std::byte> data, std::uint32_t width, std::uint32_t height,
        ImageFormat format);

    Image (std::uint32_t width, std::uint32_t height, ImageFormat format);

    void blit (const Image& src, glm::uvec2 offset);

    [[nodiscard]] inline std::uint32_t width () const noexcept {
        return m_width;
    }

    [[nodiscard]] inline std::uint32_t height () const noexcept {
        return m_height;
    }

    [[nodiscard]] inline ImageFormat format () const noexcept {
        return m_format;
    }

    [[nodiscard]] inline std::span<std::byte> data () noexcept {
        return std::span{m_data.get(), dataSize()};
    }

    [[nodiscard]] inline std::span<const std::byte> data () const noexcept {
        return std::span{m_data.get(), dataSize()};
    }

    std::size_t dataSize () const noexcept {
        return width() * height() * FormatSize(format());
    }

private:
    using DataPtr = std::unique_ptr<std::byte[], void (*)(std::byte*)>;

    DataPtr m_data;
    std::uint32_t m_width;
    std::uint32_t m_height;
    ImageFormat m_format;

    Image (DataPtr data, std::uint32_t width, std::uint32_t height, ImageFormat format);

    template<typename T>
    static DataPtr MakeData (std::size_t size) {
        auto* data = reinterpret_cast<std::byte*>(new T[size]);

        return DataPtr{data, [] (std::byte* ptr) {
                           delete[] (reinterpret_cast<T*>(ptr));
                       }};
    }
};
} // namespace phenyl::graphics
