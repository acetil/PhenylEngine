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
    private:
        VkDevice device;
        VkSwapchainKHR swapChain{};
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainViews;
        std::uint32_t currIndex{};

        VkFormat imageFormat;
        VkExtent2D imageExtent;

        void createImages ();
    public:
        VulkanSwapChain (VkDevice device, VkSurfaceKHR surface, const VulkanSwapChainDetails& details, const VulkanQueueFamilies& queueFamilies);
        ~VulkanSwapChain();

        VkFormat format () const noexcept {
            return imageFormat;
        }

        VkExtent2D extent () const noexcept {
            return imageExtent;
        }

        VkViewport getViewport () const noexcept;
        VkRect2D getScissor () const noexcept;

        std::optional<SwapChainImage> acquireImage (const VulkanSemaphore& signalSem);
        bool present (VkQueue queue, const VulkanSemaphore& waitSem);
    };
}