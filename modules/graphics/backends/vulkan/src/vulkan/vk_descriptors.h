#pragma once

#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanDescriptorPool {
    private:
        VkDevice device;
        std::vector<VkDescriptorPool> pools;
        std::size_t setsAllocated = 0;

        std::size_t poolUniformSize;

        void guaranteePool ();
    public:
        explicit VulkanDescriptorPool (VkDevice device, std::size_t poolUniformSize = 32);
        ~VulkanDescriptorPool();

        VkDescriptorSet makeDescriptorSet (VkDescriptorSetLayout layout);
        void reset ();
    };
}