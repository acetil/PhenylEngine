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
    public:
        VulkanImage () = default;
        VulkanImage (VulkanResources& resources, VkFormat format, VkImageAspectFlags aspect, VkImageUsageFlags usage, std::uint32_t width, std::uint32_t height, std::uint32_t layers = 1);

        explicit operator bool () const noexcept {
            return static_cast<bool>(m_imageInfo);
        }

        VkImage get () const noexcept {
            return m_imageInfo->image;
        }

        VkImageLayout layout () const noexcept {
            return m_layout;
        }

        VkFormat format () const noexcept {
            return m_format;
        }

        std::uint32_t width () const noexcept {
            return m_width;
        }

        std::uint32_t height () const noexcept {
            return m_height;
        }

        std::uint32_t layers () const noexcept {
            return m_layers;
        }

        bool isArray () const noexcept {
            return layers() > 1;
        }

        void layoutTransition (VulkanCommandBuffer2& cmd, VkImageLayout newLayout);

        void loadImage (TransferManager& transferManager, const graphics::Image& image, std::uint32_t layer = 0);
        void copy (TransferManager& transferManager, VulkanImage& srcImage);

    private:
        VulkanResource<VulkanImageInfo> m_imageInfo{};
        VulkanResources* m_resources = nullptr;

        VkFormat m_format = VK_FORMAT_UNDEFINED;
        VkImageAspectFlags m_aspect = VK_IMAGE_ASPECT_NONE;
        std::uint32_t m_width = 0;
        std::uint32_t m_height = 0;
        std::uint32_t m_layers = 0;

        VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    class VulkanImageView {
    public:
        VulkanImageView () = default;
        VulkanImageView (VulkanResources& resources, const VulkanImage& image, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

        explicit operator bool () const noexcept {
            return static_cast<bool>(m_view);
        }

        VkImageView get () const noexcept {
            return *m_view;
        }

    private:
        VulkanResource<VkImageView> m_view{};
        VkImageAspectFlags m_aspect;
    };

    class VulkanSampler {
    public:
        VulkanSampler (VulkanResources& resources, const graphics::TextureProperties& properties);
        VulkanSampler (VulkanResources& resources, const graphics::FrameBufferProperties& properties);

        VkSampler get () const noexcept {
            return *m_sampler;
        }

    private:
        VulkanResource<VkSampler> m_sampler;
    };

    class IVulkanCombinedSampler : public graphics::ISampler {
    public:
        virtual void prepareSampler (VulkanCommandBuffer2& cmd) = 0;
        virtual VkDescriptorImageInfo getDescriptor () const noexcept = 0;
    };

    class CombinedSampler : public IVulkanCombinedSampler {
    public:
        CombinedSampler (VulkanResources& resources, const graphics::TextureProperties& properties, VkImageLayout samplerLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
        CombinedSampler (VulkanResources& resources, const graphics::FrameBufferProperties& properties, VkImageLayout samplerLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);

        VulkanImage& image () noexcept {
            return m_image;
        }

        const VulkanImage& image () const noexcept {
            return m_image;
        }

        VkImageView view () const noexcept {
            return m_imageView.get();
        }

        explicit operator bool () const noexcept {
            return m_image && m_imageView;
        }

        void recreate (VulkanResources& resources, VulkanImage&& newImage, VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);

        [[nodiscard]] std::size_t hash () const noexcept override;

        void prepareSampler (VulkanCommandBuffer2& cmd) override;
        VkDescriptorImageInfo getDescriptor () const noexcept override;

    private:
        VulkanImage m_image{};
        VulkanImageView m_imageView{};
        VulkanSampler m_sampler;
        VkImageLayout m_samplerLayout;

    };
}
