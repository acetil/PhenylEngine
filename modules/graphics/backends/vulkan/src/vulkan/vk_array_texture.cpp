#include "vk_array_texture.h"

#include "memory/vk_buffer.h"

using namespace phenyl::vulkan;

VulkanArrayTexture::VulkanArrayTexture (VulkanResources& resources, TransferManager& transferManager,
    const graphics::TextureProperties& properties, std::uint32_t texWidth, std::uint32_t texHeight) :
    m_resources{resources},
    m_transferManager{transferManager},
    m_properties{properties},
    m_combinedSampler{resources, properties},
    m_width{texWidth},
    m_height{texHeight} {
    m_combinedSampler.recreate(resources,
        VulkanImage{resources, FormatToVulkan(properties.format), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_SAMPLED_BIT,
          texWidth, texHeight, static_cast<std::uint32_t>(m_capacity)});
}

std::uint32_t VulkanArrayTexture::width () const noexcept {
    return m_width;
}

std::uint32_t VulkanArrayTexture::height () const noexcept {
    return m_height;
}

std::uint32_t VulkanArrayTexture::size () const noexcept {
    return m_size;
}

void VulkanArrayTexture::reserve (std::uint32_t capacity) {
    if (m_capacity >= capacity) {
        return;
    }

    auto newCapacity = static_cast<std::uint32_t>(m_capacity * RESIZE_FACTOR);
    VulkanImage newImage{m_resources, FormatToVulkan(m_properties.format), VK_IMAGE_ASPECT_COLOR_BIT,
      VK_IMAGE_USAGE_SAMPLED_BIT, width(), height(), newCapacity};

    newImage.copy(m_transferManager, m_combinedSampler.image());

    m_combinedSampler.recreate(m_resources, std::move(newImage));
    m_capacity = newCapacity;
}

std::uint32_t VulkanArrayTexture::append () {
    return static_cast<std::uint32_t>(m_size++);
}

void VulkanArrayTexture::upload (std::uint32_t index, const graphics::Image& image) {
    PHENYL_ASSERT_MSG(index < m_size, "Attempted to upload image to invalid layer index {} (num layers: {})!", index,
        size());
    m_combinedSampler.image().loadImage(m_transferManager, image, index);
}

phenyl::graphics::ISampler& VulkanArrayTexture::sampler () noexcept {
    return m_combinedSampler;
}
