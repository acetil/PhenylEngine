#include "vk_storage_buffer.h"

using namespace phenyl::vulkan;

static VkBufferUsageFlags GetUsage (bool isIndex);

VulkanStorageBuffer::VulkanStorageBuffer (VulkanResources& resources, std::size_t newSize, bool isIndex) : resources{resources}, currSize{newSize}, capacity{newSize}, isIndex{isIndex} {
    if (currSize > 0) {
        buffer = VulkanBuffer{resources, GetUsage(isIndex), capacity};
    }
}

void VulkanStorageBuffer::upload (std::span<const std::byte> data) {
    if (data.empty()) {
        currSize = 0;
        return;
    }

    if (!buffer || data.size() > capacity) {
        buffer = VulkanBuffer{resources, GetUsage(isIndex), data.size()};
        capacity = data.size();
    }

    buffer.copyIn(data);
    currSize = data.size();
}

VkBuffer VulkanStorageBuffer::getBuffer () const noexcept {
    return buffer ? buffer.get() : nullptr;
}

VulkanStaticStorageBuffer::VulkanStaticStorageBuffer (VulkanResources& resources, TransferManager& transferManager, bool isIndex) : resources{resources}, transferManager{transferManager}, isIndex{isIndex} {}

void VulkanStaticStorageBuffer::upload (std::span<const std::byte> data) {
    if (data.empty()) {
        buffer = VulkanStaticBuffer{};
        return;
    }

    buffer = VulkanStaticBuffer{resources, transferManager, GetUsage(isIndex), data};
}

static VkBufferUsageFlags GetUsage (bool isIndex) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    usage |= isIndex ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    return usage;
}
