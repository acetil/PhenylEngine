#pragma once

#include "graphics/backend/texture.h"

#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanArrayTexture : public graphics::IImageArrayTexture {
    private:
        class Sampler : public graphics::ISampler {
            std::size_t hash() const noexcept override {
                return 1;
            }
        };

        Sampler dummy{};
        std::size_t texSize = 8;
    public:
        std::uint32_t width() const noexcept override;
        std::uint32_t height() const noexcept override;

        std::uint32_t size() const noexcept override;
        void reserve(std::uint32_t capacity) override;

        std::uint32_t append() override;
        void upload(std::uint32_t index, const graphics::Image& image) override;
        const graphics::ISampler& sampler() const noexcept override;
    };
}