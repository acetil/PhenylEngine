#pragma once

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

    class VulkanSwapChain {
    private:
        VkDevice device;
        VkSwapchainKHR swapChain{};
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainViews;

        VkFormat imageFormat;
        VkExtent2D extent;

        void createImages ();
    public:
        VulkanSwapChain (VkDevice device, VkSurfaceKHR surface, const VulkanSwapChainDetails& details, const VulkanQueueFamilies& queueFamilies);
        ~VulkanSwapChain();

        VkFormat format () const noexcept {
            return imageFormat;
        }
    };
}