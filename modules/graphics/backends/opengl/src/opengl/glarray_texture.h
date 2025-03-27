#pragma once

#include "glsampler.h"

namespace phenyl::opengl {
    class GlArrayTexture : public graphics::IImageArrayTexture {
    private:
        GlSampler texSampler;
        std::uint32_t texWidth;
        std::uint32_t texHeight;
        std::uint32_t depth;
        std::uint32_t depthCapacity;
    public:
        GlArrayTexture (const graphics::TextureProperties& properties, std::uint32_t texWidth, std::uint32_t texHeight);

        [[nodiscard]] std::uint32_t width () const noexcept override;
        [[nodiscard]] std::uint32_t height () const noexcept override;

        [[nodiscard]] std::uint32_t size () const noexcept override;
        void reserve (std::uint32_t capacity) override;
        std::uint32_t append () override;

        void upload (std::uint32_t index, const graphics::Image& image) override;

        const graphics::ISampler& sampler () const noexcept override;
    };
}