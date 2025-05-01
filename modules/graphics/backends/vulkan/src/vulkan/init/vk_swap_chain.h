#pragma once

#include "vulkan/vk_sync.h"
#include "vulkan/vulkan_headers.h"

namespace phenyl::vulkan {
    struct VulkanSwapChainDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct VulkanQueueFamilies {
        std::uint32_t graphicsFamily;
        std::uint32_t presentFanily;
    };

    struct SwapChainImage {
        VkImage image;
        VkImageView view;
    };

    class VulkanSwapChain {
    public:
        VulkanSwapChain (VkDevice device, VkSurfaceKHR surface, const VulkanSwapChainDetails& details, const VulkanQueueFamilies& queueFamilies);
        ~VulkanSwapChain();

        VkFormat format () const noexcept {
            return m_format;
        }

        VkExtent2D extent () const noexcept {
            return m_extent;
        }

        VkViewport viewport () const noexcept;
        VkRect2D scissor () const noexcept;

        std::optional<SwapChainImage> acquireImage (const VulkanSemaphore& signalSem);
        bool present (VkQueue queue, const VulkanSemaphore& waitSem);

    private:
        VkDevice m_device;
        VkSwapchainKHR m_swapChain{};
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;
        std::uint32_t m_currIndex{};

        VkFormat m_format;
        VkExtent2D m_extent;

        void createImages ();
    };
}