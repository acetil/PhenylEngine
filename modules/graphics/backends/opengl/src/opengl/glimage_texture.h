#pragma once

#include "graphics/backend/texture.h"
#include "opengl_headers.h"
#include "glsampler.h"

namespace phenyl::graphics {
    class GlImageTexture : public IImageTexture {
    private:
        GlSampler texSampler;
        std::uint32_t texWidth{0};
        std::uint32_t texHeight{0};
    public:
        explicit GlImageTexture (const TextureProperties& properties);

        [[nodiscard]] std::uint32_t width () const noexcept override;
        [[nodiscard]] std::uint32_t height () const noexcept override;

        void upload (const phenyl::graphics::Image& image) override;
        const ISampler& sampler() const noexcept override;
    };
}