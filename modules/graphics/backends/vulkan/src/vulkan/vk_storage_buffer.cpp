#include "vk_storage_buffer.h"

using namespace phenyl::vulkan;

static VkBufferUsageFlags GetUsage (bool isIndex);

VulkanStorageBuffer::VulkanStorageBuffer (VulkanResources& resources, std::size_t newSize, bool isIndex) : m_resources{resources}, m_size{newSize}, m_capacity{newSize}, m_isIndex{isIndex} {
    if (m_size > 0) {
        m_buffer = VulkanBuffer{resources, GetUsage(isIndex), m_capacity};
    }
}

void VulkanStorageBuffer::upload (std::span<const std::byte> data) {
    if (data.empty()) {
        m_size = 0;
        return;
    }

    if (!m_buffer || data.size() > m_capacity) {
        m_buffer = VulkanBuffer{m_resources, GetUsage(m_isIndex), data.size()};
        m_capacity = data.size();
    }

    m_buffer.copyIn(data);
    m_size = data.size();
}

VkBuffer VulkanStorageBuffer::getBuffer () const noexcept {
    return m_buffer ? m_buffer.get() : nullptr;
}

VulkanStaticStorageBuffer::VulkanStaticStorageBuffer (VulkanResources& resources, TransferManager& transferManager, bool isIndex) : m_resources{resources}, m_transferManager{transferManager}, m_isIndex{isIndex} {}

void VulkanStaticStorageBuffer::upload (std::span<const std::byte> data) {
    if (data.empty()) {
        m_buffer = VulkanStaticBuffer{};
        return;
    }

    m_buffer = VulkanStaticBuffer{m_resources, m_transferManager, GetUsage(m_isIndex), data};
}

static VkBufferUsageFlags GetUsage (bool isIndex) {
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    usage |= isIndex ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    return usage;
}
