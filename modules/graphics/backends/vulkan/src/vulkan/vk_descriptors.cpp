#include "vk_descriptors.h"

using namespace phenyl::vulkan;

VulkanDescriptorPool::VulkanDescriptorPool (VkDevice device, std::size_t poolUniformSize) : device{device}, poolUniformSize{poolUniformSize} {

}

VulkanDescriptorPool::~VulkanDescriptorPool () {
    for (auto i : pools) {
        vkDestroyDescriptorPool(device, i, nullptr);
    }
}

VkDescriptorSet VulkanDescriptorPool::makeDescriptorSet (VkDescriptorSetLayout layout) {
    guaranteePool();

    VkDescriptorSetAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pools.back(),
        .descriptorSetCount = 1,
        .pSetLayouts = &layout
    };
    VkDescriptorSet set;
    if (auto result = vkAllocateDescriptorSets(device, &allocInfo, &set); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to allocate VkDescriptorSet: {}", result);
    }
    PHENYL_DASSERT(set);
    setsAllocated++;

    return set;
}

void VulkanDescriptorPool::reset () {
    for (auto i : pools) {
        vkResetDescriptorPool(device, i, 0);
    }

    setsAllocated = 0;
}

void VulkanDescriptorPool::guaranteePool () {
    if (setsAllocated / poolUniformSize < pools.size()) {
        return;
    }

    // TODO: more pools
    VkDescriptorPoolSize poolSize{
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = static_cast<std::uint32_t>(poolUniformSize) * 8 // TODO
    };

    VkDescriptorPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<std::uint32_t>(poolUniformSize),
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    VkDescriptorPool pool;
    if (auto result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool); result != VK_SUCCESS) {
        PHENYL_ABORT("Error allocating VkDescriptorPool: {}", result);
    }
    PHENYL_DASSERT(pool);

    pools.emplace_back(pool);
}
