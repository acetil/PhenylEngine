#include "vk_array_texture.h"

#include "memory/vk_buffer.h"

using namespace phenyl::vulkan;

VulkanArrayTexture::VulkanArrayTexture (VulkanResources& resources, TransferManager& transferManager, const graphics::TextureProperties& properties, std::uint32_t texWidth,
    std::uint32_t texHeight) : resources{resources}, transferManager{transferManager}, properties{properties}, combinedSampler{resources, properties}, texWidth{texWidth}, texHeight{texHeight} {
    combinedSampler.recreate(resources, VulkanImage{resources, FormatToVulkan(properties.format), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_SAMPLED_BIT, texWidth, texHeight, static_cast<std::uint32_t>(texCapacity)});
}

std::uint32_t VulkanArrayTexture::width () const noexcept {
    return texWidth;
}

std::uint32_t VulkanArrayTexture::height () const noexcept {
    return texHeight;
}

std::uint32_t VulkanArrayTexture::size () const noexcept {
    return texSize;
}

void VulkanArrayTexture::reserve (std::uint32_t capacity) {
    if (texCapacity >= capacity) {
        return;
    }

    auto newCapacity = static_cast<std::uint32_t>(texCapacity * RESIZE_FACTOR);
    VulkanImage newImage{resources, FormatToVulkan(properties.format), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_SAMPLED_BIT, width(), height(), newCapacity};

    newImage.copy(transferManager, combinedSampler.image());

    combinedSampler.recreate(resources, std::move(newImage));
    texCapacity = newCapacity;
}

std::uint32_t VulkanArrayTexture::append () {
    return static_cast<std::uint32_t>(texSize++);
}

void VulkanArrayTexture::upload (std::uint32_t index, const graphics::Image& image) {
    PHENYL_ASSERT_MSG(index < texSize, "Attempted to upload image to invalid layer index {} (num layers: {})!", index, size());
    combinedSampler.image().loadImage(transferManager, image, index);
}

phenyl::graphics::ISampler& VulkanArrayTexture::sampler () noexcept {
    return combinedSampler;
}
