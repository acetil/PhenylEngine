#pragma once

#include "graphics/renderers/texture.h"
#include "graphics/graphics_headers.h"

namespace phenyl::graphics {
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

        const TextureProperties& properties () {
            return samplerProperties;
        }

        GLenum type () const noexcept {
            return samplerType;
        }

        void bind () const;
    };
}