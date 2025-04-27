#pragma once

#include <memory>
#include "vulkan/vk_command_buffer.h"

#include "vulkan/vulkan_headers.h"
#include "vk_swap_chain.h"

namespace phenyl::vulkan {
    struct DeviceProperties {
        std::string deviceName;
        float maxAnisotropy;

        VkDeviceSize minUniformAlignment;
    };

    class VulkanDevice {
    private:
        static std::optional<VulkanQueueFamilies> GetDeviceFamilies (VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceExtensionSupport (VkPhysicalDevice device, const std::vector<const char*>& extensions);
        static std::optional<VulkanSwapChainDetails> GetDeviceSwapChainDetails (VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceFeatures (VkPhysicalDevice device);
        static DeviceProperties GetDeviceProperties (VkPhysicalDevice device);

        VkPhysicalDevice physicalDevice{};
        VkDevice logicalDevice{};

        VulkanQueueFamilies queueFamilies{};
        VulkanSwapChainDetails swapChainDetails{};
        DeviceProperties devProperties{};

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        void choosePhysicalDevice (VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
        VkDevice createLogicalDevice (const std::vector<const char*>& deviceExtensions);
        VkQueue makeQueue (std::uint32_t queueFamilyIndex);
    public:
        explicit VulkanDevice (VkInstance instance, VkSurfaceKHR surface);

        std::unique_ptr<VulkanSwapChain> makeSwapChain (VkSurfaceKHR surface);
        VkCommandPool makeCommandPool (VkCommandPoolCreateFlags usage);

        VmaAllocator makeVmaAllocator (VkInstance instance, std::uint32_t vkVersion);

        VkDevice device () const noexcept {
            return logicalDevice;
        }

        const DeviceProperties& properties () const noexcept {
            return devProperties;
        }

        VkQueue getGraphicsQueue () const noexcept {
            return graphicsQueue;
        }

        ~VulkanDevice ();
    };
}