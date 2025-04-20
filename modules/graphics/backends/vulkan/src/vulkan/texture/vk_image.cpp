#include "vk_image.h"

#include "vulkan/init/vk_device.h"
#include "vulkan/memory/vk_buffer.h"

using namespace phenyl::vulkan;

VkFormat phenyl::vulkan::FormatToVulkan (graphics::ImageFormat imageFormat) {
    switch (imageFormat) {
        case graphics::ImageFormat::R:
            return VK_FORMAT_R8_UNORM;
        case graphics::ImageFormat::RGBA:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case graphics::ImageFormat::RGBA32:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:
            PHENYL_ABORT("Unimplemented image format: {}", static_cast<std::uint32_t>(imageFormat));
    }
}

static VkFilter MagFilterToVulkan (phenyl::graphics::TextureFilter filter) {
    switch (filter) {
        case phenyl::graphics::TextureFilter::POINT:
            return VK_FILTER_NEAREST;
        case phenyl::graphics::TextureFilter::BILINEAR:
            return VK_FILTER_LINEAR;
        case phenyl::graphics::TextureFilter::TRILINEAR:
            return VK_FILTER_LINEAR; // TODO
        default:
            PHENYL_ABORT("Unexpected filter type: {}", static_cast<std::uint32_t>(filter));
    }
}

static VkSamplerAddressMode WrappingToVulkan (phenyl::graphics::TextureWrapping wrapping) {
    switch (wrapping) {
        case phenyl::graphics::TextureWrapping::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case phenyl::graphics::TextureWrapping::REPEAT_MIRROR:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case phenyl::graphics::TextureWrapping::CLAMP:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case phenyl::graphics::TextureWrapping::CLAMP_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            PHENYL_ABORT("Unexpected wrapping type: {}", static_cast<std::uint32_t>(wrapping));
    }
}

VulkanImage::VulkanImage (VulkanResources& resources, VkFormat format, std::uint32_t width, std::uint32_t height, std::uint32_t layers) : imgFormat{format}, imgWidth{width}, imgHeight{height}, imgLayers{layers}, resources{&resources} {
    PHENYL_ASSERT_MSG(width > 0 && height > 0, "Attempted to create vulkan image with invalid dimensions: {}x{}", width, height);
    PHENYL_ASSERT_MSG(layers > 0, "Attempted to create vulkan image with no layers");

    VkImageCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = VkExtent3D{
            .width = width,
            .height = height,
            .depth = 1
        },
        .mipLevels = 1, // TODO
        .arrayLayers = layers,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE, // TODO
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocCreateInfo{
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    imageInfo = resources.makeImage(createInfo, allocCreateInfo);
    PHENYL_ASSERT_MSG(imageInfo, "Failed to create image!");

    currLayout = VK_IMAGE_LAYOUT_UNDEFINED;
}

void VulkanImage::layoutTransition (VulkanCommandBuffer2& cmd, VkImageLayout newLayout) {
    if (currLayout == newLayout) {
        return;
    }

    cmd.doImageTransition(get(), currLayout, newLayout);
    currLayout = newLayout;
}

void VulkanImage::loadImage (TransferManager& transferManager, const graphics::Image& image, std::uint32_t layer) {
    PHENYL_ASSERT(imageInfo);
    PHENYL_ASSERT(resources);

    PHENYL_ASSERT_MSG(format() == FormatToVulkan(image.format()), "Attempted to load image with incorrect format!");
    PHENYL_ASSERT_MSG(width() == image.width() && height() == image.height(),
        "Attempted to load image with unexpected dimensions: expected {}x{}, got {}x{}", width(), height(), image.width(), image.height());
    PHENYL_ASSERT_MSG(layer < layers(), "Attempted to load image to invalid layer: {} (num layers: {})", layer, layers());

    auto imgData = image.data();

    VulkanBuffer buffer{*resources, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, imgData.size()};
    buffer.copyIn(imgData.data(), imgData.size());

    transferManager.queueTransfer(std::move([&] (VulkanCommandBuffer2& cmd) {
         layoutTransition(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        cmd.copyBufferToImage(buffer.get(), get(), currLayout, {width(), height(), 1}, layer);

        layoutTransition(cmd, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
    }));
}

void VulkanImage::copy (TransferManager& transferManager, VulkanImage& srcImage, bool retainSrcLayout) {
    PHENYL_ASSERT(*this);
    PHENYL_ASSERT(srcImage);

    auto numLayers = std::min(srcImage.layers(), layers());
    PHENYL_ASSERT_MSG(width() == srcImage.width() && height() == srcImage.height(),
        "Attempted to copy image with incorrect dimensions, this: {}x{}, src: {}x{}", width(), height(), srcImage.width(), srcImage.height());

    transferManager.queueTransfer([&] (VulkanCommandBuffer2& cmd) {
        auto oldSrcLayout = srcImage.layout();

        srcImage.layoutTransition(cmd, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        layoutTransition(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        cmd.copyImage(get(), layout(), srcImage.get(), srcImage.layout(), {width(), height(), 1}, numLayers);

        layoutTransition(cmd, VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
        if (retainSrcLayout) {
            srcImage.layoutTransition(cmd, oldSrcLayout);
        }
    });
}

VulkanImageView::VulkanImageView (VulkanResources& resources, const VulkanImage& image) {
    VkImageViewCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.get(),
        .viewType = image.isArray() ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D,
        .format = image.format(),
        // TODO
        .subresourceRange = VkImageSubresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = image.layers()
        }
    };

    imageView = resources.makeImageView(createInfo);
    PHENYL_ASSERT_MSG(imageView, "Failed to create image view");
}

VulkanSampler::VulkanSampler (VulkanResources& resources, const graphics::TextureProperties& properties) {
    VkSamplerCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = MagFilterToVulkan(properties.filter),
        .minFilter = VK_FILTER_NEAREST, // TODO
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR, // TODO
        .addressModeU = WrappingToVulkan(properties.wrapping),
        .addressModeV = WrappingToVulkan(properties.wrapping),
        .addressModeW = WrappingToVulkan(properties.wrapping),
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = resources.getDeviceProperties().maxAnisotropy,
        .compareEnable = VK_FALSE, // TODO
        .compareOp = VK_COMPARE_OP_ALWAYS, // TODO
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, // TODO
        .unnormalizedCoordinates = VK_FALSE
    };

    sampler = resources.makeSampler(createInfo);
    PHENYL_ASSERT_MSG(sampler, "Failed to create image sampler");
}

CombinedSampler::CombinedSampler (VulkanResources& resources, const graphics::TextureProperties& properties) : sampler{resources, properties} {}

void CombinedSampler::recreate (VulkanResources& resources, VulkanImage&& newImage) {
    samplerImage = std::move(newImage);
    samplerImageView = VulkanImageView{resources, samplerImage};
}

std::size_t CombinedSampler::hash () const noexcept {
    return reinterpret_cast<std::size_t>(sampler.get());
}

VkDescriptorImageInfo CombinedSampler::getDescriptor () const noexcept {
    return {
        .sampler = sampler.get(),
        .imageView = samplerImageView.get(),
        .imageLayout = image().layout()
    };
}
