#pragma once

#include "graphics/backend/texture.h"
#include "opengl_headers.h"
#include "glsampler.h"

namespace phenyl::opengl {
    class GlImageTexture : public graphics::IImageTexture {
    private:
        GlSampler m_sampler;
        std::uint32_t m_width{0};
        std::uint32_t m_height{0};
    public:
        explicit GlImageTexture (const graphics::TextureProperties& properties);

        [[nodiscard]] std::uint32_t width () const noexcept override;
        [[nodiscard]] std::uint32_t height () const noexcept override;

        void upload (const phenyl::graphics::Image& image) override;
        graphics::ISampler& sampler() noexcept override;
    };
}