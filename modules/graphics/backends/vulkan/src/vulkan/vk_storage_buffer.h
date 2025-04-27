#pragma once

#include "graphics/backend/buffer.h"

#include "memory/vk_buffer.h"
#include "vk_renderer.h"

namespace phenyl::vulkan {
    class IVulkanStorageBuffer : public graphics::IBuffer {
    public:
        virtual VkBuffer getBuffer () const noexcept = 0;
    };

    class VulkanStorageBuffer : public IVulkanStorageBuffer {
    private:
        VulkanResources& resources;
        VulkanBuffer buffer{};
        std::size_t currSize;
        std::size_t capacity;
        bool isIndex;

    public:
        VulkanStorageBuffer (VulkanResources& resources, std::size_t size, bool isIndex);

        void upload (unsigned char* data, std::size_t size) override;
        std::size_t size () const noexcept {
            return currSize;
        }

        VkBuffer getBuffer () const noexcept override;
    };

    class VulkanStaticStorageBuffer : public IVulkanStorageBuffer {
    private:
        VulkanResources& resources;
        TransferManager& transferManager;
        VulkanStaticBuffer buffer{};
        bool isIndex;

    public:
        VulkanStaticStorageBuffer (VulkanResources& resources, TransferManager& transferManager, bool isIndex);

        void upload (unsigned char* data, std::size_t size) override;

        std::size_t size () const noexcept {
            return buffer.size();
        }

        VkBuffer getBuffer () const noexcept override {
            return buffer.get();
        }
    };
}
