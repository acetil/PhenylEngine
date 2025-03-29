#pragma once

#include "graphics/backend/texture.h"

namespace phenyl::vulkan {
    class VulkanImageTexture : public graphics::IImageTexture {
    private:
        class Sampler : public graphics::ISampler {
            std::size_t hash () const noexcept override {
                return 1;
            }
        };

        Sampler dummy{};
    public:
        std::uint32_t width () const noexcept override;
        std::uint32_t height () const noexcept override;

        void upload (const graphics::Image& image) override;
        const graphics::ISampler& sampler () const noexcept override;
    };
}