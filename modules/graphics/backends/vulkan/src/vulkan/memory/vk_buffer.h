#pragma once

#include "vulkan/vulkan_headers.h"
#include "vulkan/init/vk_resources.h"

namespace phenyl::vulkan {
    class VulkanBuffer {
    private:
        VulkanResource<VulkanBufferInfo> bufferInfo;

        std::size_t bufSize;

    public:
        VulkanBuffer () = default;
        VulkanBuffer (VulkanResources& resources, VkBufferUsageFlags usage, std::size_t bufSize);

        explicit operator bool () const noexcept {
            return static_cast<bool>(bufferInfo);
        }

        VkBuffer get () const noexcept {
            PHENYL_DASSERT(*this);
            return bufferInfo->buffer;
        }

        std::uint32_t size () const noexcept {
            return bufSize;
        }

        void copyIn (const std::byte* data, std::size_t size, std::size_t off = 0);
    };
}
