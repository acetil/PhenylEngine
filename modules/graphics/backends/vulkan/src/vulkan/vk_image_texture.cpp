#include "vk_image_texture.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_IMAGE_TEXTURE", detail::VULKAN_LOGGER};

VulkanImageTexture::VulkanImageTexture (VulkanResources& resources, TransferManager& transferManager, const graphics::TextureProperties& properties) : resources{resources}, transferManager{transferManager}, combinedSampler{resources, properties} {}

std::uint32_t VulkanImageTexture::width () const noexcept {
    return combinedSampler ? combinedSampler.image().width() : 0;
}

std::uint32_t VulkanImageTexture::height () const noexcept {
    return combinedSampler ? combinedSampler.image().height() : 0;
}

void VulkanImageTexture::upload (const graphics::Image& image) {
    recreateIfNecessary(FormatToVulkan(image.format()), image.width(), image.height());

    PHENYL_DASSERT(combinedSampler);
    combinedSampler.image().loadImage(transferManager, image);
}

phenyl::graphics::ISampler& VulkanImageTexture::sampler () noexcept {
    PHENYL_ASSERT_MSG(combinedSampler, "Attempted to get sampler of uninitialised image");
    return combinedSampler;
}

void VulkanImageTexture::recreateIfNecessary (VkFormat format, std::uint32_t width, std::uint32_t height) {
    if (!combinedSampler) {
        recreateSampler(format, width, height);
        return;
    }

    if (this->width() != width) {
        recreateSampler(format, width, height);
        return;
    }

    if (this->height() != height) {
        recreateSampler(format, width, height);
        return;
    }

    if (combinedSampler.image().format() != format) {
        recreateSampler(format, width, height);
    }
}

void VulkanImageTexture::recreateSampler (VkFormat imageFormat, std::uint32_t width, std::uint32_t height) {
    PHENYL_LOGD(LOGGER, "Recreating VulkanImage with dimensions {}x{} and format {}", width, height, imageFormat);

    VulkanImage newImage{resources, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_USAGE_SAMPLED_BIT, width, height};
    combinedSampler.recreate(resources, std::move(newImage));
}
