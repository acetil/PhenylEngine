#pragma once

#include "graphics/backend/uniform_buffer.h"
#include "memory/vk_buffer.h"

namespace phenyl::vulkan {
class VulkanRenderer;

class VulkanUniformBuffer : public graphics::IUniformBuffer {
public:
    VulkanUniformBuffer (VulkanResources& resources);
    std::span<std::byte> allocate (std::size_t size) override;
    void upload () override;
    bool isReadable () const override;
    std::size_t getMinAlignment () const noexcept override;

    VkBuffer getBuffer () const;

    const VkDescriptorBufferInfo& getBufferInfo () const noexcept {
        return m_bufferInfo;
    }

    [[nodiscard]] VkDescriptorBufferInfo getBufferInfo (std::size_t offset, std::size_t size) const noexcept;

private:
    VulkanResources& m_resources;

    std::unique_ptr<std::byte[]> m_data;
    std::size_t m_size = 0;

    VulkanBuffer m_buffer;
    VkDescriptorBufferInfo m_bufferInfo;
};
} // namespace phenyl::vulkan
