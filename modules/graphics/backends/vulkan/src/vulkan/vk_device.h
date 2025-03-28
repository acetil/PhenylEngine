#pragma once

#include "vulkan_headers.h"

namespace phenyl::vulkan {
    struct VulkanQueueFamilies {
        std::uint32_t graphicsFamily;
    };

    class VulkanDevice {
    private:
        static std::optional<VulkanQueueFamilies> GetDeviceFamilies (VkPhysicalDevice device);

        VkPhysicalDevice physicalDevice{};
        VkDevice logicalDevice{};

        VkQueue graphicsQueue;

        VulkanQueueFamilies choosePhysicalDevice (VkInstance instance);
        VkDevice createLogicalDevice (const VulkanQueueFamilies& queueFamiles);
    public:
        explicit VulkanDevice (VkInstance instance);
        ~VulkanDevice ();
    };
}