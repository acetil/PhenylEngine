#pragma once

#include <memory>
#include <vulkan/vk_command_buffer.h>

#include "vulkan/vulkan_headers.h"
#include "vk_swap_chain.h"

namespace phenyl::vulkan {
    class VulkanDevice {
    private:
        static std::optional<VulkanQueueFamilies> GetDeviceFamilies (VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceExtensionSupport (VkPhysicalDevice device, const std::vector<const char*>& extensions);
        static std::optional<VulkanSwapChainDetails> GetDeviceSwapChainDetails (VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceFeatures (VkPhysicalDevice device);

        VkPhysicalDevice physicalDevice{};
        VkDevice logicalDevice{};
        VulkanQueueFamilies queueFamilies{};
        VulkanSwapChainDetails swapChainDetails{};

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        void choosePhysicalDevice (VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
        VkDevice createLogicalDevice (const std::vector<const char*>& deviceExtensions);
        VkQueue makeQueue (std::uint32_t queueFamilyIndex);
    public:
        explicit VulkanDevice (VkInstance instance, VkSurfaceKHR surface);

        std::unique_ptr<VulkanSwapChain> makeSwapChain (VkSurfaceKHR surface);
        std::unique_ptr<VulkanCommandPool> makeCommandPool (std::size_t initialCapacity = 1);

        VkDevice device () const noexcept {
            return logicalDevice;
        }

        VkQueue getGraphicsQueue () const noexcept {
            return graphicsQueue;
        }

        ~VulkanDevice ();
    };
}