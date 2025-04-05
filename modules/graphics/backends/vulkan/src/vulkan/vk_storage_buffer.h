#pragma once

#include "graphics/backend/buffer.h"

#include "memory/vk_buffer.h"
#include "vk_renderer.h"

namespace phenyl::vulkan {
    class VulkanStorageBuffer : public graphics::IBuffer {
    private:
        VulkanRenderer& renderer;
        std::unique_ptr<VulkanBuffer> buffer;
        std::size_t currSize;
        std::size_t capacity;
        bool isIndex;
    public:
        VulkanStorageBuffer (VulkanRenderer& renderer, std::size_t size, bool isIndex);

        void upload (unsigned char* data, std::size_t size) override;
        std::size_t size () const noexcept {
            return currSize;
        }

        VkBuffer getBuffer () const;
    };
}
