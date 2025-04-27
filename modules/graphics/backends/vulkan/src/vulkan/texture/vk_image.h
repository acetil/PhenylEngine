#pragma once
#include "vulkan/memory/vk_transfer_manager.h"
#include "graphics/image.h"
#include "graphics/backend/framebuffer.h"
#include "graphics/backend/texture.h"
#include "vulkan/vk_command_buffer.h"
#include "vulkan/init/vk_resources.h"

namespace phenyl::vulkan {
    VkFormat FormatToVulkan (graphics::ImageFormat imageFormat);

    class VulkanImage {
    private:
        VulkanResource<VulkanImageInfo> imageInfo{};
        VulkanResources* resources = nullptr;

        VkFormat imgFormat = VK_FORMAT_UNDEFINED;
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_NONE;
        std::uint32_t imgWidth = 0;
        std::uint32_t imgHeight = 0;
        std::uint32_t imgLayers = 0;

        VkImageLayout currLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    public:
        VulkanImage () = default;
        VulkanImage (VulkanResources& resources, VkFormat format, VkImageAspectFlags aspect, VkImageUsageFlags usage, std::uint32_t width, std::uint32_t height, std::uint32_t layers = 1);

        explicit operator bool () const noexcept {
            return static_cast<bool>(imageInfo);
        }

        VkImage get () const noexcept {
            return imageInfo->image;
        }

        VkImageLayout layout () const noexcept {
            return currLayout;
        }

        VkFormat format () const noexcept {
            return imgFormat;
        }

        std::uint32_t width () const noexcept {
            return imgWidth;
        }

        std::uint32_t height () const noexcept {
            return imgHeight;
        }

        std::uint32_t layers () const noexcept {
            return imgLayers;
        }

        bool isArray () const noexcept {
            return layers() > 1;
        }

        void layoutTransition (VulkanCommandBuffer2& cmd, VkImageLayout newLayout);

        void loadImage (TransferManager& transferManager, const graphics::Image& image, std::uint32_t layer = 0);
        void copy (TransferManager& transferManager, VulkanImage& srcImage);
    };

    class VulkanImageView {
    private:
        VulkanResource<VkImageView> imageView{};
        VkImageAspectFlags imageAspect;

    public:
        VulkanImageView () = default;
        VulkanImageView (VulkanResources& resources, const VulkanImage& image, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

        explicit operator bool () const noexcept {
            return static_cast<bool>(imageView);
        }

        VkImageView get () const noexcept {
            return *imageView;
        }
    };

    class VulkanSampler {
    private:
        VulkanResource<VkSampler> sampler;

    public:
        VulkanSampler (VulkanResources& resources, const graphics::TextureProperties& properties);
        VulkanSampler (VulkanResources& resources, const graphics::FrameBufferProperties& properties);

        VkSampler get () const noexcept {
            return *sampler;
        }
    };

    class IVulkanCombinedSampler : public graphics::ISampler {
    public:
        virtual void prepareSampler (VulkanCommandBuffer2& cmd) = 0;
        virtual VkDescriptorImageInfo getDescriptor () const noexcept = 0;
    };

    class CombinedSampler : public IVulkanCombinedSampler {
    private:
        VulkanImage samplerImage{};
        VulkanImageView samplerImageView{};
        VulkanSampler sampler;
        VkImageLayout samplerLayout;

    public:
        CombinedSampler (VulkanResources& resources, const graphics::TextureProperties& properties, VkImageLayout samplerLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
        CombinedSampler (VulkanResources& resources, const graphics::FrameBufferProperties& properties, VkImageLayout samplerLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);

        VulkanImage& image () noexcept {
            return samplerImage;
        }

        const VulkanImage& image () const noexcept {
            return samplerImage;
        }

        VkImageView view () const noexcept {
            return samplerImageView.get();
        }

        explicit operator bool () const noexcept {
            return samplerImage && samplerImageView;
        }

        void recreate (VulkanResources& resources, VulkanImage&& newImage, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

        [[nodiscard]] std::size_t hash () const noexcept override;

        void prepareSampler (VulkanCommandBuffer2& cmd) override;
        VkDescriptorImageInfo getDescriptor () const noexcept override;
    };
}
