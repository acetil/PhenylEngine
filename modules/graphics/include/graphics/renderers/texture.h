#pragma once

#include <memory>

#include "graphics/image.h"

namespace phenyl::graphics {
    enum class TextureFilter {
        POINT,
        BILINEAR,
        TRILINEAR
    };

    struct TextureProperties {
        ImageFormat format = ImageFormat::R;
        TextureFilter filter = TextureFilter::POINT;
        bool useMipmapping = true;
    };

    class ITexture {
    public:
        virtual ~ITexture() = default;

        [[nodiscard]] virtual std::size_t hash () const noexcept = 0;

        [[nodiscard]] virtual std::uint32_t width () const noexcept = 0;
        [[nodiscard]] virtual std::uint32_t height () const noexcept = 0;
    };

    class IImageTexture : public ITexture {
    public:
        virtual void upload (const Image& image) = 0;
    };

    class Texture {
    protected:
        std::unique_ptr<ITexture> rendererTexture;
        std::size_t textureHash;

    public:
        static constexpr std::size_t INVALID_HASH = -1;

        Texture () : rendererTexture{}, textureHash{INVALID_HASH} {}
        explicit Texture (std::unique_ptr<ITexture> rendererTexture) : rendererTexture{std::move(rendererTexture)}, textureHash{this->rendererTexture->hash()} {}

        explicit operator bool () const noexcept {
            return (bool)rendererTexture;
        }

        [[nodiscard]] inline std::uint32_t width () const noexcept {
            return rendererTexture->width();
        }

        [[nodiscard]] inline std::uint32_t height () const noexcept {
            return rendererTexture->height();
        }

        [[nodiscard]] inline std::size_t hash () const noexcept {
            return textureHash;
        }

        [[nodiscard]] ITexture& getUnderlying () noexcept {
            return *rendererTexture;
        }

        [[nodiscard]] const ITexture& getUnderlying () const noexcept {
            return *rendererTexture;
        }
    };

    class ImageTexture : public Texture {
    private:
        IImageTexture& getRef () {
            return static_cast<IImageTexture&>(*rendererTexture);
        }
    public:
        ImageTexture () = default;
        explicit ImageTexture (std::unique_ptr<IImageTexture> texture) : Texture{std::move(texture)} {}

        void upload (const Image& image) {
            getRef().upload(image);
        }
    };
}