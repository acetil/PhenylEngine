#pragma once

#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanDescriptorPool {
    public:
        explicit VulkanDescriptorPool (VkDevice device, std::size_t poolUniformSize = 32);
        ~VulkanDescriptorPool();

        VkDescriptorSet makeDescriptorSet (VkDescriptorSetLayout layout);
        void reset ();

    private:
        VkDevice m_device;
        std::vector<VkDescriptorPool> m_pools;
        std::size_t m_numAllocated = 0;

        std::size_t m_poolCapacity;

        void guaranteePool ();
    };
}