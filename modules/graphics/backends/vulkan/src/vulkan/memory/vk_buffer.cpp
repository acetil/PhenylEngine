#include "vk_buffer.h"

using namespace phenyl::vulkan;

VulkanBuffer::VulkanBuffer (VulkanResources& resources, VkBufferUsageFlags usage, std::size_t bufSize) : bufSize{bufSize} {
    PHENYL_ASSERT(bufSize > 0);
    // TODO: different flags
    VkBufferCreateInfo bufCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = static_cast<VkDeviceSize>(bufSize),
        .usage = usage
    };

    VmaAllocationCreateInfo allocCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };

    bufferInfo = resources.makeBuffer(bufCreateInfo, allocCreateInfo);
    PHENYL_ASSERT_MSG(bufferInfo, "Failed to create buffer!");
}

void VulkanBuffer::copyIn (const std::byte* data, std::size_t size, std::size_t off) {
    PHENYL_ASSERT(off < bufSize);
    PHENYL_ASSERT(off + size <= bufSize);
    PHENYL_ASSERT(bufferInfo);
    PHENYL_ASSERT(data);

    auto result = vmaCopyMemoryToAllocation(bufferInfo->allocator, data, bufferInfo->alloc, static_cast<VkDeviceSize>(off), static_cast<VkDeviceSize>(size));
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to copy memory to allocation: {}", result);

    result = vmaFlushAllocation(bufferInfo->allocator, bufferInfo->alloc, static_cast<VkDeviceSize>(off), static_cast<VkDeviceSize>(size));
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to flush memory alocation: {}", result);
}
