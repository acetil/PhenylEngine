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
    public:
        explicit VulkanDevice (VkInstance instance, VkSurfaceKHR surface);

        std::unique_ptr<VulkanSwapChain> makeSwapChain (VkSurfaceKHR surface);
        VkCommandPool makeCommandPool (VkCommandPoolCreateFlags usage);

        VmaAllocator makeVmaAllocator (VkInstance instance, std::uint32_t vkVersion);

        VkDevice device () const noexcept {
            return m_logicalDevice;
        }

        const DeviceProperties& properties () const noexcept {
            return m_properties;
        }

        VkQueue graphicsQueue () const noexcept {
            return m_graphicsQueue;
        }

        ~VulkanDevice ();

    private:
        static std::optional<VulkanQueueFamilies> GetDeviceFamilies (VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceExtensionSupport (VkPhysicalDevice device, const std::vector<const char*>& extensions);
        static std::optional<VulkanSwapChainDetails> GetDeviceSwapChainDetails (VkPhysicalDevice device, VkSurfaceKHR surface);
        static bool CheckDeviceFeatures (VkPhysicalDevice device);
        static DeviceProperties GetDeviceProperties (VkPhysicalDevice device);

        VkPhysicalDevice m_physicalDevice{};
        VkDevice m_logicalDevice{};

        VulkanQueueFamilies m_queueFamilies{};
        VulkanSwapChainDetails m_swapChainDetails{};
        DeviceProperties m_properties{};

        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;

        void choosePhysicalDevice (VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
        VkDevice createLogicalDevice (const std::vector<const char*>& deviceExtensions);
        VkQueue makeQueue (std::uint32_t queueFamilyIndex);
    };
}