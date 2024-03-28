#pragma once

#include "graphics/renderers/texture.h"
#include "graphics/graphics_headers.h"

namespace phenyl::graphics {
    class GlTexture : public IImageTexture {
    private:
        GLuint textureId{0};
        TextureProperties properties;
        std::uint32_t texWidth{0};
        std::uint32_t texHeight{0};
    public:
        explicit GlTexture (const TextureProperties& properties);

        GlTexture (const GlTexture&) = delete;
        GlTexture (GlTexture&& other) noexcept;

        GlTexture& operator= (const GlTexture&) = delete;
        GlTexture& operator= (GlTexture&& other) noexcept;

        ~GlTexture() override;

        [[nodiscard]] std::size_t hash () const noexcept override;
        [[nodiscard]] std::uint32_t width () const noexcept override;
        [[nodiscard]] std::uint32_t height () const noexcept override;

        void upload (const phenyl::graphics::Image& image) override;

        [[nodiscard]] inline GLuint id () const noexcept {
            return textureId;
        }
    };
}