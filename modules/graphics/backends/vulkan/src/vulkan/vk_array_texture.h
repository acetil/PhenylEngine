#pragma once

#include "graphics/backend/texture.h"
#include "texture/vk_image.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
class VulkanArrayTexture : public graphics::IImageArrayTexture {
public:
    VulkanArrayTexture (VulkanResources& resources, TransferManager& transferManager,
        const graphics::TextureProperties& properties, std::uint32_t texWidth, std::uint32_t texHeight);

    std::uint32_t width () const noexcept override;
    std::uint32_t height () const noexcept override;

    std::uint32_t size () const noexcept override;
    void reserve (std::uint32_t capacity) override;

    std::uint32_t append () override;
    void upload (std::uint32_t index, const graphics::Image& image) override;
    graphics::ISampler& sampler () noexcept override;

private:
    static constexpr std::uint32_t STARTING_CAPACITY = 8;
    static constexpr double RESIZE_FACTOR = 3.0 / 2.0;

    VulkanResources& m_resources;
    TransferManager& m_transferManager;
    graphics::TextureProperties m_properties;

    CombinedSampler m_combinedSampler;

    std::uint32_t m_width;
    std::uint32_t m_height;

    std::size_t m_size = 0;
    std::size_t m_capacity = STARTING_CAPACITY;
};
} // namespace phenyl::vulkan
