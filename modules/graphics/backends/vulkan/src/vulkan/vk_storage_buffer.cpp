#include "vk_storage_buffer.h"

using namespace phenyl::vulkan;

VulkanStorageBuffer::VulkanStorageBuffer (VulkanRenderer& renderer, std::size_t newSize, bool isIndex) : renderer{renderer}, currSize{newSize}, capacity{newSize}, isIndex{isIndex} {
    if (currSize > 0) {
        buffer = std::make_unique<VulkanBuffer>(renderer.makeBuffer(newSize, isIndex));
    }
}

void VulkanStorageBuffer::upload (unsigned char* data, std::size_t size) {
    if (size == 0) {
        currSize = 0;
        return;
    }

    if (!buffer) {
        buffer = std::make_unique<VulkanBuffer>(renderer.makeBuffer(size, isIndex));
        capacity = size;
    } else if (size > capacity) {
        // TODO
        *buffer = renderer.makeBuffer(size, isIndex);
        capacity = size;
    }

    if (data) {
        buffer->copyIn(reinterpret_cast<std::byte*>(data), size);
    }
    currSize = size;
}

VkBuffer VulkanStorageBuffer::getBuffer () const {
    return buffer ? buffer->get() : nullptr;
}
