#include "vk_buffer.h"

using namespace phenyl::vulkan;

VulkanBuffer::VulkanBuffer (VmaAllocator allocator, VkBufferUsageFlags usage, std::size_t bufSize) : allocator{allocator}, bufSize{bufSize} {
    PHENYL_ASSERT(bufSize > 0);
    // TODO: different flags
    VkBufferCreateInfo bufCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = static_cast<VkDeviceSize>(bufSize),
        .usage = usage | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
    };

    VmaAllocationCreateInfo allocCreateInfo{
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO,
    };
    auto result = vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &buffer, &alloc, &allocInfo);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create buffer using VMA: {}", result);

    PHENYL_DASSERT(buffer);
    PHENYL_DASSERT(alloc);
}

VulkanBuffer::VulkanBuffer (VulkanBuffer&& other) noexcept : allocator{other.allocator}, buffer{other.buffer},
        alloc{other.alloc}, allocInfo{other.allocInfo}, bufSize{other.bufSize} {
    other.allocator = nullptr;
    other.buffer = nullptr;
    other.alloc = nullptr;
    other.bufSize = 0;
}

VulkanBuffer& VulkanBuffer::operator= (VulkanBuffer&& other) noexcept {
    if (buffer) {
        PHENYL_DASSERT(allocator);
        vmaDestroyBuffer(allocator, buffer, alloc);
    }

    allocator = other.allocator;
    buffer = other.buffer;
    alloc = other.alloc;
    allocInfo = other.allocInfo;
    bufSize = other.bufSize;

    other.allocator = nullptr;
    other.buffer = nullptr;
    other.alloc = nullptr;
    other.bufSize = 0;

    return *this;
}

VulkanBuffer::~VulkanBuffer () {
    if (buffer) {
        PHENYL_DASSERT(allocator);
        vmaDestroyBuffer(allocator, buffer, alloc);
    }
}

void VulkanBuffer::copyIn (std::byte* data, std::size_t size, std::size_t off) {
    PHENYL_ASSERT(off < bufSize);
    PHENYL_ASSERT(off + size <= bufSize);
    PHENYL_ASSERT(allocator);
    PHENYL_ASSERT(alloc);
    PHENYL_ASSERT(data);

    auto result = vmaCopyMemoryToAllocation(allocator, data, alloc, static_cast<VkDeviceSize>(off), static_cast<VkDeviceSize>(size));
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to copy memory to allocation: {}", result);

    result = vmaFlushAllocation(allocator, alloc, static_cast<VkDeviceSize>(off), static_cast<VkDeviceSize>(size));
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to flush memory alocation: {}", result);
}
