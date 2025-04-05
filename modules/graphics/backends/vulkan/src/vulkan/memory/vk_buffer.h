#pragma once

#include "vulkan/vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanBuffer {
    private:
        VmaAllocator allocator;

        VkBuffer buffer{};
        VmaAllocation alloc{};
        VmaAllocationInfo allocInfo{};

        std::size_t bufSize;

    public:
        VulkanBuffer (VmaAllocator allocator, VkBufferUsageFlags usage, std::size_t bufSize);
        VulkanBuffer (const VulkanBuffer&) = delete;
        VulkanBuffer (VulkanBuffer&&) noexcept;

        VulkanBuffer& operator= (const VulkanBuffer&) = delete;
        VulkanBuffer& operator= (VulkanBuffer&&) noexcept;

        ~VulkanBuffer ();

        VkBuffer get () const noexcept {
            return buffer;
        }

        std::uint32_t size () const noexcept {
            return bufSize;
        }

        void copyIn (std::byte* data, std::size_t size, std::size_t off = 0);
    };
}