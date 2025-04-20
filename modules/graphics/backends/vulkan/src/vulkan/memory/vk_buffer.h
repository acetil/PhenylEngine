#pragma once

#include "vulkan/vulkan_headers.h"
#include "vulkan/init/vk_resources.h"

namespace phenyl::vulkan {
    class VulkanBuffer {
    private:
        VulkanResource<VulkanBufferInfo> bufferInfo;

        std::size_t bufSize;

    public:
        VulkanBuffer (VulkanResources& resources, VkBufferUsageFlags usage, std::size_t bufSize);

        VkBuffer get () const noexcept {
            return bufferInfo->buffer;
        }

        std::uint32_t size () const noexcept {
            return bufSize;
        }

        void copyIn (const std::byte* data, std::size_t size, std::size_t off = 0);
    };
}
