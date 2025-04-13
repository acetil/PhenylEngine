#pragma once

#include "graphics/backend/uniform_buffer.h"
#include "memory/vk_buffer.h"

namespace phenyl::vulkan {
    class VulkanRenderer;

    class VulkanUniformBuffer : public graphics::IUniformBuffer {
    private:
        VulkanRenderer& renderer;
        std::unique_ptr<std::byte[]> data;
        std::size_t size = 0;

        std::unique_ptr<VulkanBuffer> buffer;
        VkDescriptorBufferInfo bufferInfo;

    public:
        VulkanUniformBuffer (VulkanRenderer& renderer);
        unsigned char* allocate (std::size_t size) override;
        void upload () override;
        bool isReadable () const override;

        VkBuffer getBuffer () const;

        const VkDescriptorBufferInfo& getBufferInfo () const noexcept {
            return bufferInfo;
        }
    };
}