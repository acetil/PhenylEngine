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
public:
    VulkanStorageBuffer (VulkanResources& resources, std::size_t size, bool isIndex);

    void upload (std::span<const std::byte> data) override;

    std::size_t size () const noexcept {
        return m_size;
    }

    VkBuffer getBuffer () const noexcept override;

private:
    VulkanResources& m_resources;
    VulkanBuffer m_buffer{};
    std::size_t m_size;
    std::size_t m_capacity;
    bool m_isIndex;
};

class VulkanStaticStorageBuffer : public IVulkanStorageBuffer {
public:
    VulkanStaticStorageBuffer (VulkanResources& resources, TransferManager& transferManager, bool isIndex);

    void upload (std::span<const std::byte> data) override;

    std::size_t size () const noexcept {
        return m_buffer.size();
    }

    VkBuffer getBuffer () const noexcept override {
        return m_buffer.get();
    }

private:
    VulkanResources& m_resources;
    TransferManager& m_transferManager;
    VulkanStaticBuffer m_buffer{};
    bool m_isIndex;
};
} // namespace phenyl::vulkan
