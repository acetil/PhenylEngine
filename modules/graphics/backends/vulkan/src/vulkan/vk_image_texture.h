#pragma once

#include "graphics/backend/texture.h"
#include "texture/vk_image.h"

namespace phenyl::vulkan {
    class VulkanImageTexture : public graphics::IImageTexture {
    private:
        VulkanResources& resources;
        TransferManager& transferManager;

        CombinedSampler combinedSampler;

        void recreateIfNecessary (VkFormat format, std::uint32_t width, std::uint32_t height);
        void recreateSampler (VkFormat imageFormat, std::uint32_t width, std::uint32_t height);
    public:
        VulkanImageTexture (VulkanResources& resources, TransferManager& transferManager, const graphics::TextureProperties& properties);

        std::uint32_t width () const noexcept override;
        std::uint32_t height () const noexcept override;

        void upload (const graphics::Image& image) override;
        const graphics::ISampler& sampler () const noexcept override;
    };
}
