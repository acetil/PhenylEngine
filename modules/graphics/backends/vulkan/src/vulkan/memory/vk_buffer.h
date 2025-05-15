#pragma once

#include "vk_transfer_manager.h"
#include "vulkan/init/vk_resources.h"
#include "vulkan/vulkan_headers.h"

namespace phenyl::vulkan {
class VulkanBuffer {
public:
    VulkanBuffer () = default;
    VulkanBuffer (VulkanResources& resources, VkBufferUsageFlags usage, std::size_t bufSize);

    explicit operator bool () const noexcept {
        return static_cast<bool>(m_bufferInfo);
    }

    VkBuffer get () const noexcept {
        PHENYL_DASSERT(*this);
        return m_bufferInfo->buffer;
    }

    std::uint32_t size () const noexcept {
        return m_size;
    }

    void copyIn (std::span<const std::byte> data, std::size_t off = 0);

private:
    VulkanResource<VulkanBufferInfo> m_bufferInfo;

    std::size_t m_size;
};

class VulkanStaticBuffer {
public:
    VulkanStaticBuffer () = default;
    VulkanStaticBuffer (VulkanResources& resources, TransferManager& transferManager, VkBufferUsageFlags usage,
        std::span<const std::byte> data);

    explicit operator bool () const noexcept {
        return static_cast<bool>(m_bufferInfo);
    }

    VkBuffer get () const noexcept {
        PHENYL_DASSERT(*this);
        return m_bufferInfo->buffer;
    }

    std::size_t size () const noexcept {
        return m_size;
    }

private:
    VulkanResource<VulkanBufferInfo> m_bufferInfo{};
    std::size_t m_size = 0;
};
} // namespace phenyl::vulkan
