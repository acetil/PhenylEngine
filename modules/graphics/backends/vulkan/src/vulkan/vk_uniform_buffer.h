#pragma once

#include "graphics/backend/uniform_buffer.h"
#include "memory/vk_buffer.h"

namespace phenyl::vulkan {
    class VulkanRenderer;

    class VulkanUniformBuffer : public graphics::IUniformBuffer {
    private:
        VulkanResources& resources;

        std::unique_ptr<std::byte[]> data;
        std::size_t size = 0;

        VulkanBuffer buffer;
        VkDescriptorBufferInfo bufferInfo;

    public:
        VulkanUniformBuffer (VulkanResources& resources);
        unsigned char* allocate (std::size_t size) override;
        void upload () override;
        bool isReadable () const override;
        std::size_t getMinAlignment () const noexcept override;

        VkBuffer getBuffer () const;

        const VkDescriptorBufferInfo& getBufferInfo () const noexcept {
            return bufferInfo;
        }

        [[nodiscard]] VkDescriptorBufferInfo getBufferInfo (std::size_t offset, std::size_t size) const noexcept;
    };
}