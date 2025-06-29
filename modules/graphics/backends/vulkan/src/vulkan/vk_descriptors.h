#pragma once

#include "init/vk_resources.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
struct DescriptorSizes {
    std::uint32_t uniforms = 0;
    std::uint32_t samplers = 0;
};

class VulkanDescriptorPool {
public:
    explicit VulkanDescriptorPool (VulkanResources& resources, DescriptorSizes sizes, std::size_t poolCapacity = 32);

    VkDescriptorSet makeDescriptorSet (VkDescriptorSetLayout layout);
    void reset ();

private:
    VulkanResources& m_resources;
    std::vector<VulkanResource<VkDescriptorPool>> m_pools;
    std::size_t m_numAllocated = 0;

    DescriptorSizes m_sizes;
    std::size_t m_poolCapacity;

    void guaranteePool ();
};
} // namespace phenyl::vulkan
