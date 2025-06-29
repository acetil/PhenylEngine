#include "vk_descriptors.h"

using namespace phenyl::vulkan;

VulkanDescriptorPool::VulkanDescriptorPool (VulkanResources& resources, DescriptorSizes sizes,
    std::size_t poolCapacity) :
    m_resources{resources},
    m_sizes{sizes},
    m_poolCapacity{poolCapacity} {}

VkDescriptorSet VulkanDescriptorPool::makeDescriptorSet (VkDescriptorSetLayout layout) {
    guaranteePool();

    VkDescriptorSetAllocateInfo allocInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .descriptorPool = *m_pools.back(),
      .descriptorSetCount = 1,
      .pSetLayouts = &layout,
    };
    VkDescriptorSet set;
    if (auto result = vkAllocateDescriptorSets(m_resources.getDevice(), &allocInfo, &set); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to allocate VkDescriptorSet: {}", result);
    }
    PHENYL_DASSERT(set);
    m_numAllocated++;

    return set;
}

void VulkanDescriptorPool::reset () {
    for (const auto& i : m_pools) {
        vkResetDescriptorPool(m_resources.getDevice(), *i, 0);
    }

    m_numAllocated = 0;
}

void VulkanDescriptorPool::guaranteePool () {
    if (m_numAllocated / m_poolCapacity < m_pools.size()) {
        return;
    }

    std::vector<VkDescriptorPoolSize> poolSizes;
    if (m_sizes.uniforms > 0) {
        poolSizes.emplace_back(VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .descriptorCount = m_sizes.uniforms * static_cast<std::uint32_t>(m_poolCapacity)});
    }
    if (m_sizes.samplers > 0) {
        poolSizes.emplace_back(VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .descriptorCount = m_sizes.samplers * static_cast<std::uint32_t>(m_poolCapacity)});
    }

    VkDescriptorPoolCreateInfo poolInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .maxSets = static_cast<std::uint32_t>(m_poolCapacity),
      .poolSizeCount = static_cast<std::uint32_t>(poolSizes.size()),
      .pPoolSizes = poolSizes.data()};
    m_pools.emplace_back(m_resources.makeDescriptorPool(poolInfo));
    PHENYL_ASSERT(m_pools.back());
}
