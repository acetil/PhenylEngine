#pragma once

#include "graphics/backend/texture.h"
#include "opengl_headers.h"

namespace phenyl::opengl {
inline GLint GetGlInternalFormat (graphics::ImageFormat format) {
    switch (format) {
    case graphics::ImageFormat::R:
        return GL_RED;
    case graphics::ImageFormat::RGBA:
        return GL_RGBA;
    case graphics::ImageFormat::RGBA32:
        return GL_RGBA32F;
    case graphics::ImageFormat::DEPTH24_STENCIL8:
        return GL_DEPTH24_STENCIL8;
    case graphics::ImageFormat::DEPTH:
        return GL_DEPTH_COMPONENT;
    }

    PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
}

inline GLint GetGLFormat (graphics::ImageFormat format) {
    switch (format) {
    case graphics::ImageFormat::R:
        return GL_RED;
    case graphics::ImageFormat::RGBA:
    case graphics::ImageFormat::RGBA32:
        return GL_RGBA;
    case graphics::ImageFormat::DEPTH24_STENCIL8:
        return GL_DEPTH_STENCIL;
    case graphics::ImageFormat::DEPTH:
        return GL_DEPTH_COMPONENT;
    }

    PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
}

static GLenum GetGlFormatType (graphics::ImageFormat format) {
    switch (format) {
    case graphics::ImageFormat::R:
    case graphics::ImageFormat::RGBA:
        return GL_UNSIGNED_BYTE;
    case graphics::ImageFormat::DEPTH24_STENCIL8:
        return GL_UNSIGNED_INT_24_8;
    case graphics::ImageFormat::DEPTH:
        return GL_FLOAT;
    case graphics::ImageFormat::RGBA32:
        return GL_FLOAT;
    }

    PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
}

static GLint GetGlWrapping (graphics::TextureWrapping wrapping) {
    switch (wrapping) {
    case graphics::TextureWrapping::REPEAT:
        return GL_REPEAT;
    case graphics::TextureWrapping::REPEAT_MIRROR:
        return GL_MIRRORED_REPEAT;
    case graphics::TextureWrapping::CLAMP:
        return GL_CLAMP_TO_EDGE;
    case graphics::TextureWrapping::CLAMP_BORDER:
        return GL_CLAMP_TO_BORDER;
    }

    PHENYL_ABORT("Invalid wrapping: {}", static_cast<std::uint32_t>(wrapping));
}

static glm::vec4 GetGlBorderColor (graphics::TextureBorderColor color) {
    switch (color) {
    case graphics::TextureBorderColor::TRANSPARENT:
        return {0, 0, 0, 0};
    case graphics::TextureBorderColor::BLACK:
        return {0, 0, 0, 1};
    case graphics::TextureBorderColor::WHITE:
        return {1, 1, 1, 1};
    }

    PHENYL_ABORT("Invalid border color: {}", static_cast<std::uint32_t>(color));
}

class GlSampler : public graphics::ISampler {
public:
    explicit GlSampler (GLenum samplerType, const graphics::TextureProperties& properties);
    GlSampler (const GlSampler&) = delete;
    GlSampler (GlSampler&& other) noexcept;

    GlSampler& operator= (const GlSampler&) = delete;
    GlSampler& operator= (GlSampler&& other) noexcept;

    ~GlSampler () override;

    [[nodiscard]] GLuint id () const noexcept {
        return m_id;
    }

    [[nodiscard]] std::size_t hash () const noexcept override;

    const graphics::TextureProperties& properties () const noexcept {
        return m_properties;
    }

    GLenum type () const noexcept {
        return m_type;
    }

    GLint internalFormat () const noexcept;
    GLint format () const noexcept;
    GLenum formatType () const noexcept;
    GLint filter () const noexcept;

    void createEmpty2D (std::uint32_t width, std::uint32_t height);
    void upload (const graphics::Image& image);

    void createEmpty3D (std::uint32_t width, std::uint32_t height, std::uint32_t depth);
    void uploadLayer (std::uint32_t layer, const graphics::Image& image);

    void bind () const;

protected:
    GLuint m_id{0};
    GLenum m_type;
    graphics::TextureProperties m_properties;
};
} // namespace phenyl::opengl
