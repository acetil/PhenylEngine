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
    VulkanSemaphore* semaphore;
};

class VulkanSwapChain {
public:
    VulkanSwapChain (VulkanResources& resources, VkSurfaceKHR surface, const VulkanSwapChainDetails& details,
        const VulkanQueueFamilies& queueFamilies);
    ~VulkanSwapChain ();

    VkFormat format () const noexcept {
        return m_format;
    }

    VkExtent2D extent () const noexcept {
        return m_extent;
    }

    VkViewport viewport () const noexcept;
    VkRect2D scissor () const noexcept;

    std::optional<SwapChainImage> acquireImage (const VulkanSemaphore& signalSem);
    const VulkanSemaphore* imageSemaphore () const;
    bool present (VkQueue queue);

private:
    VkDevice m_device;
    VkSwapchainKHR m_swapChain{};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<VulkanSemaphore> m_semaphores;
    std::uint32_t m_currIndex{};

    VkFormat m_format;
    VkExtent2D m_extent;

    void createImages (VulkanResources& resources);
};
} // namespace phenyl::vulkan
