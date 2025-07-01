#include "vk_buffer.h"

using namespace phenyl::vulkan;

VulkanBuffer::VulkanBuffer (VulkanResources& resources, VkBufferUsageFlags usage, std::size_t bufSize) :
    m_size{bufSize} {
    PHENYL_ASSERT(bufSize > 0);
    // TODO: different flags
    VkBufferCreateInfo bufCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = static_cast<VkDeviceSize>(bufSize),
      .usage = usage,
    };

    VmaAllocationCreateInfo allocCreateInfo{
      .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
      .usage = VMA_MEMORY_USAGE_AUTO,
    };

    m_bufferInfo = resources.makeBuffer(bufCreateInfo, allocCreateInfo);
    PHENYL_ASSERT_MSG(m_bufferInfo, "Failed to create buffer!");
}

void VulkanBuffer::copyIn (std::span<const std::byte> data, std::size_t off) {
    PHENYL_DASSERT(*this);
    PHENYL_ASSERT(off < m_size);
    PHENYL_ASSERT(off + data.size() <= m_size);
    PHENYL_ASSERT(m_bufferInfo);

    if (data.empty()) {
        return;
    }

    auto result = vmaCopyMemoryToAllocation(m_bufferInfo->allocator, data.data(), m_bufferInfo->alloc,
        static_cast<VkDeviceSize>(off), static_cast<VkDeviceSize>(data.size()));
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to copy memory to allocation: {}", result);

    result = vmaFlushAllocation(m_bufferInfo->allocator, m_bufferInfo->alloc, static_cast<VkDeviceSize>(off),
        static_cast<VkDeviceSize>(data.size()));
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to flush memory alocation: {}", result);
}

VulkanStaticBuffer::VulkanStaticBuffer (VulkanResources& resources, TransferManager& transferManager,
    VkBufferUsageFlags usage, std::span<const std::byte> data) :
    m_size{data.size()} {
    PHENYL_ASSERT(!data.empty());

    VkBufferCreateInfo bufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .size = static_cast<VkDeviceSize>(data.size()),
      .usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    };

    VmaAllocationCreateInfo allocCreateInfo{
      .usage = VMA_MEMORY_USAGE_AUTO,
      .preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    m_bufferInfo = resources.makeBuffer(bufferCreateInfo, allocCreateInfo);
    PHENYL_ASSERT_MSG(m_bufferInfo, "Failed to create GPU buffer");

    VulkanBuffer stagingBuffer{resources, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, data.size()};
    stagingBuffer.copyIn(data);

    transferManager.queueTransfer(
        [&] (VulkanCommandBuffer2& cmd) { cmd.copyBuffer(stagingBuffer.get(), 0, get(), 0, data.size()); });
}
