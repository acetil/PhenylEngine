#pragma once

#include "graphics/backend/texture.h"
#include "texture/vk_image.h"

namespace phenyl::vulkan {
class VulkanImageTexture : public graphics::IImageTexture {
public:
    VulkanImageTexture (VulkanResources& resources, TransferManager& transferManager,
        const graphics::TextureProperties& properties);

    std::uint32_t width () const noexcept override;
    std::uint32_t height () const noexcept override;

    void upload (const graphics::Image& image) override;
    graphics::ISampler& sampler () noexcept override;

private:
    VulkanResources& m_resources;
    TransferManager& m_transferManager;

    CombinedSampler m_combinedSampler;

    void recreateIfNecessary (VkFormat format, std::uint32_t width, std::uint32_t height);
    void recreateSampler (VkFormat imageFormat, std::uint32_t width, std::uint32_t height);
};
} // namespace phenyl::vulkan
