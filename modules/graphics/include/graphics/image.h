#pragma once

#include <string>
#include <memory>

#include "graphics/maths_headers.h"
#include "util/optional.h"

namespace phenyl::graphics {
    enum class ImageFormat {
        R,
        RGBA,
        RGBA32,
        DEPTH24_STENCIL8,
        DEPTH
    };

    class Image {
    private:
        using DataPtr = std::unique_ptr<std::byte[], void (*) (std::byte*)>;

        DataPtr imgData;
        std::uint32_t imgWidth;
        std::uint32_t imgHeight;
        ImageFormat imgFormat;

        Image (DataPtr data, std::uint32_t width, std::uint32_t height, ImageFormat format);

        template <typename T>
        static DataPtr MakeData (std::size_t size) {
            auto* data = reinterpret_cast<std::byte*>(new T[size]);

            return DataPtr{data, [] (std::byte* ptr) {
                delete[] (reinterpret_cast<T*>(ptr));
            }};
        }
    public:
        static std::uint32_t FormatComps (ImageFormat format);
        static std::size_t FormatSize (ImageFormat format);

        static util::Optional<Image> Load (std::istream& file, ImageFormat format = ImageFormat::RGBA);
        static Image MakeNonOwning (std::span<std::byte> data, std::uint32_t width, std::uint32_t height, ImageFormat format);

        Image (std::uint32_t width, std::uint32_t height, ImageFormat format);

        void blit (const Image& src, glm::uvec2 offset);

        [[nodiscard]] inline std::uint32_t width () const noexcept {
            return imgWidth;
        }

        [[nodiscard]] inline std::uint32_t height () const noexcept {
            return imgHeight;
        }

        [[nodiscard]] inline ImageFormat format () const noexcept {
            return imgFormat;
        }

        [[nodiscard]] inline std::span<std::byte> data () noexcept {
            return std::span{imgData.get(), width() * height() * FormatSize(format())};
        }

        [[nodiscard]] inline std::span<const std::byte> data () const noexcept {
            return std::span{imgData.get(), width() * height() * FormatSize(format())};
        }
    };
}
