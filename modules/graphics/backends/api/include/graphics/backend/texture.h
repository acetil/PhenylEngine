#pragma once

#include <memory>

#include "graphics/image.h"

namespace phenyl::graphics {

    // TODO: move sampler to per-pipeline?
    enum class TextureFilter {
        POINT,
        BILINEAR,
        TRILINEAR
    };

    enum class TextureWrapping {
        REPEAT,
        REPEAT_MIRROR,
        CLAMP,
        CLAMP_BORDER
    };

    struct TextureProperties {
        ImageFormat format = ImageFormat::R;
        TextureFilter filter = TextureFilter::POINT;
        TextureWrapping wrapping = TextureWrapping::REPEAT;
        glm::vec4 borderColor = {0, 0, 0, 0};
        bool useMipmapping = true;
    };

    class ISampler {
    public:
        virtual ~ISampler () = default;

        [[nodiscard]] virtual std::size_t hash () const noexcept = 0;
    };

    class IImageTexture {
    public:
        virtual ~IImageTexture () = default;

        [[nodiscard]] virtual std::uint32_t width () const noexcept = 0;
        [[nodiscard]] virtual std::uint32_t height () const noexcept = 0;

        virtual void upload (const Image& image) = 0;
        [[nodiscard]] virtual ISampler& sampler () noexcept = 0;
    };

    class IImageArrayTexture {
    public:
        virtual ~IImageArrayTexture () = default;

        [[nodiscard]] virtual std::uint32_t width () const noexcept = 0;
        [[nodiscard]] virtual std::uint32_t height () const noexcept = 0;

        [[nodiscard]] virtual std::uint32_t size () const noexcept = 0;
        virtual void reserve (std::uint32_t capacity)  = 0;

        virtual std::uint32_t append () = 0;

        virtual void upload (std::uint32_t index, const Image& image) = 0;
        [[nodiscard]] virtual ISampler& sampler () noexcept = 0;
    };

    class Texture {
    private:
        std::size_t texHash;
    public:
        Texture () : texHash{0} {}
        explicit Texture (std::size_t hash) : texHash{hash} {}

        virtual ~Texture() = default;

        [[nodiscard]] virtual ISampler& sampler () const noexcept = 0;

        [[nodiscard]] std::size_t hash () const noexcept {
            return texHash;
        }
    };

    class ImageTexture : public Texture {
    private:
        std::unique_ptr<IImageTexture> rendererTexture;
    public:
        ImageTexture () = default;
        explicit ImageTexture (std::unique_ptr<IImageTexture> texture) : Texture{0}, rendererTexture{std::move(texture)} {}

        explicit operator bool () const noexcept {
            return (bool)rendererTexture;
        }

        [[nodiscard]] inline std::uint32_t width () const noexcept {
            return rendererTexture->width();
        }

        [[nodiscard]] inline std::uint32_t height () const noexcept {
            return rendererTexture->height();
        }

        void upload (const Image& image) {
            rendererTexture->upload(image);
        }

        [[nodiscard]] ISampler& sampler () const noexcept override {
            return rendererTexture->sampler();
        }
    };

    class ImageArrayTexture : public Texture {
    private:
        std::unique_ptr<IImageArrayTexture> rendererTexture;
    public:
        ImageArrayTexture () = default;
        explicit ImageArrayTexture (std::unique_ptr<IImageArrayTexture> texture) : Texture{texture->sampler().hash()}, rendererTexture{std::move(texture)}{}

        explicit operator bool () const noexcept {
            return (bool)rendererTexture;
        }

        [[nodiscard]] inline std::uint32_t width () const noexcept {
            return rendererTexture->width();
        }

        [[nodiscard]] inline std::uint32_t height () const noexcept {
            return rendererTexture->height();
        }

        [[nodiscard]] std::uint32_t size () const noexcept {
            return rendererTexture->size();
        }

        void reserve (std::uint32_t capacity) {
            rendererTexture->reserve(capacity);
        }

        std::uint32_t append () {
            return rendererTexture->append();
        }

        void upload (std::uint32_t index, const Image& image) {
            rendererTexture->upload(index, image);
        }

        [[nodiscard]] ISampler& sampler() const noexcept override {
            return rendererTexture->sampler();
        }
    };
}