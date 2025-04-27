#include "vk_storage_buffer.h"

using namespace phenyl::vulkan;

static VkBufferUsageFlags GetUsage (bool isIndex);

VulkanStorageBuffer::VulkanStorageBuffer (VulkanResources& resources, std::size_t newSize, bool isIndex) : resources{resources}, currSize{newSize}, capacity{newSize}, isIndex{isIndex} {
    if (currSize > 0) {
        buffer = VulkanBuffer{resources, GetUsage(isIndex), capacity};
    }
}

void VulkanStorageBuffer::upload (unsigned char* data, std::size_t size) {
    if (size == 0) {
        currSize = 0;
        return;
    }

    if (!buffer || size > capacity) {
        buffer = VulkanBuffer{resources, GetUsage(isIndex), size};
        capacity = size;
    }

    if (data) {
        buffer.copyIn(reinterpret_cast<std::byte*>(data), size);
    }
    currSize = size;
}

VkBuffer VulkanStorageBuffer::getBuffer () const noexcept {
    return buffer ? buffer.get() : nullptr;
}

VulkanStaticStorageBuffer::VulkanStaticStorageBuffer (VulkanResources& resources, TransferManager& transferManager, bool isIndex) : resources{resources}, transferManager{transferManager}, isIndex{isIndex} {}

void VulkanStaticStorageBuffer::upload (unsigned char* data, std::size_t size) {
    buffer = VulkanStaticBuffer{resources, transferManager, GetUsage(isIndex), std::span{reinterpret_cast<std::byte*>(data), size}};
}

static VkBufferUsageFlags GetUsage (bool isIndex) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    usage |= isIndex ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    return usage;
}
