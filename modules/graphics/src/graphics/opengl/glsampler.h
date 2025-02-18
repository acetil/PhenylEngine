#pragma once

#include "graphics/texture.h"
#include "graphics/graphics_headers.h"

namespace phenyl::graphics {
    inline GLint GetGlInternalFormat (ImageFormat format) {
        switch (format) {
            case ImageFormat::R:
                return GL_RED;
            case ImageFormat::RGBA:
                return GL_RGBA;
            case ImageFormat::DEPTH24_STENCIL8:
                return GL_DEPTH24_STENCIL8;
        }

        PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
    }

    inline GLint GetGLFormat (ImageFormat format) {
        switch (format) {
            case ImageFormat::R:
                return GL_RED;
            case ImageFormat::RGBA:
                return GL_RGBA;
            case ImageFormat::DEPTH24_STENCIL8:
                return GL_DEPTH_STENCIL;
        }

        PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
    }

    static GLenum GetGlFormatType (ImageFormat format) {
        switch (format) {
            case ImageFormat::R:
            case ImageFormat::RGBA:
                return GL_UNSIGNED_BYTE;
            case ImageFormat::DEPTH24_STENCIL8:
                return GL_UNSIGNED_INT_24_8;
        }

        PHENYL_ABORT("Invalid format type: {}", static_cast<std::uint32_t>(format));
    }

    class GlSampler : public ISampler {
    protected:
        GLuint textureId{0};
        GLenum samplerType;
        TextureProperties samplerProperties;

    public:
        explicit GlSampler (GLenum samplerType, const TextureProperties& properties);
        GlSampler (const GlSampler&) = delete;
        GlSampler (GlSampler&& other) noexcept;

        GlSampler& operator= (const GlSampler&) = delete;
        GlSampler& operator= (GlSampler&& other) noexcept;

        ~GlSampler() override;

        [[nodiscard]] GLuint id () const noexcept {
            return textureId;
        }

        [[nodiscard]] std::size_t hash () const noexcept override;

        const TextureProperties& properties () const noexcept {
            return samplerProperties;
        }

        GLenum type () const noexcept {
            return samplerType;
        }

        GLint internalFormat () const noexcept;
        GLint format () const noexcept;
        GLenum formatType () const noexcept;
        GLint filter () const noexcept;

        void createEmpty2D (std::uint32_t width, std::uint32_t height);
        void upload (const Image& image);

        void createEmpty3D (std::uint32_t width, std::uint32_t height, std::uint32_t depth);
        void uploadLayer (std::uint32_t layer, const Image& image);

        void bind () const;
    };
}