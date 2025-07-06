#pragma once

#include "core/assets/asset.h"
#include "graphics/image.h"

#include <memory>

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

enum class TextureBorderColor {
    TRANSPARENT,
    BLACK,
    WHITE
};

struct TextureProperties {
    ImageFormat format = ImageFormat::R;
    TextureFilter filter = TextureFilter::POINT;
    TextureWrapping wrapping = TextureWrapping::REPEAT;
    TextureBorderColor borderColor = TextureBorderColor::TRANSPARENT;
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
    virtual void reserve (std::uint32_t capacity) = 0;

    virtual std::uint32_t append () = 0;

    virtual void upload (std::uint32_t index, const Image& image) = 0;
    [[nodiscard]] virtual ISampler& sampler () noexcept = 0;
};

class Texture : public core::AssetType2<Texture> {
public:
    Texture () : m_hash{0} {}

    explicit Texture (std::size_t hash) : m_hash{hash} {}

    virtual ~Texture () = default;

    [[nodiscard]] virtual ISampler& sampler () const noexcept = 0;

    [[nodiscard]] std::size_t hash () const noexcept {
        return m_hash;
    }

private:
    std::size_t m_hash;
};

class ImageTexture : public Texture {
public:
    ImageTexture () = default;

    explicit ImageTexture (std::unique_ptr<IImageTexture> texture) : Texture{0}, m_texture{std::move(texture)} {}

    explicit operator bool () const noexcept {
        return (bool) m_texture;
    }

    [[nodiscard]] inline std::uint32_t width () const noexcept {
        return m_texture->width();
    }

    [[nodiscard]] inline std::uint32_t height () const noexcept {
        return m_texture->height();
    }

    void upload (const Image& image) {
        m_texture->upload(image);
    }

    [[nodiscard]] ISampler& sampler () const noexcept override {
        return m_texture->sampler();
    }

private:
    std::unique_ptr<IImageTexture> m_texture;
};

class ImageArrayTexture : public Texture {
public:
    ImageArrayTexture () = default;

    explicit ImageArrayTexture (std::unique_ptr<IImageArrayTexture> texture) :
        Texture{texture->sampler().hash()},
        m_texture{std::move(texture)} {}

    explicit operator bool () const noexcept {
        return (bool) m_texture;
    }

    [[nodiscard]] inline std::uint32_t width () const noexcept {
        return m_texture->width();
    }

    [[nodiscard]] inline std::uint32_t height () const noexcept {
        return m_texture->height();
    }

    [[nodiscard]] std::uint32_t size () const noexcept {
        return m_texture->size();
    }

    void reserve (std::uint32_t capacity) {
        m_texture->reserve(capacity);
    }

    std::uint32_t append () {
        return m_texture->append();
    }

    void upload (std::uint32_t index, const Image& image) {
        m_texture->upload(index, image);
    }

    [[nodiscard]] ISampler& sampler () const noexcept override {
        return m_texture->sampler();
    }

private:
    std::unique_ptr<IImageArrayTexture> m_texture;
};
} // namespace phenyl::graphics
