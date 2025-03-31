#pragma once

#include "vk_sync.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanRenderingRecorder;

    class VulkanRecordedCommandBuffer {
    private:
        VkCommandBuffer commandBuffer;

    public:
        explicit VulkanRecordedCommandBuffer (VkCommandBuffer commandBuffer);

        void submit (VkQueue queue, const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence& fence);
    };

    class VulkanCommandBuffer {
    private:
        VkCommandBuffer commandBuffer;
        bool inRecorder = false;

        void endRendering ();

        friend class VulkanRenderingRecorder;
    public:
        explicit VulkanCommandBuffer (VkCommandBuffer commandBuffer);

        VulkanRenderingRecorder beginRendering (VkImageView imageView, VkImageLayout imageLayout, VkExtent2D drawExtent, std::optional<VkClearValue> clearColor = std::nullopt);

        void doImageTransition (VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

        VulkanRecordedCommandBuffer record ();
        void drop ();
    };

    class VulkanRenderingRecorder {
    private:
        VulkanCommandBuffer& owner;
        VkCommandBuffer commandBuffer;

        VkPipeline currPipeline = nullptr;
        std::optional<VkViewport> currViewport{};
        std::optional<VkRect2D> currScissor;
    public:
        VulkanRenderingRecorder (VulkanCommandBuffer& owner, VkCommandBuffer commandBuffer);

        void setPipeline (VkPipeline pipeline, VkViewport viewport, VkRect2D scissor);
        void draw (std::uint32_t instanceCount, std::uint32_t vertexCount, std::uint32_t firstVertex);
        void draw (std::uint32_t vertexCount, std::uint32_t firstVertex);

        ~VulkanRenderingRecorder ();
    };

    class VulkanCommandPool {
    private:
        VkDevice device;
        VkCommandPool commandPool;

        std::vector<VkCommandBuffer> availableBuffers;
        std::vector<VkCommandBuffer> usedBuffers;

        void addBuffers (std::size_t count);
    public:
        VulkanCommandPool (VkDevice device, std::uint32_t queueIndex, std::size_t capacity = 1);
        VulkanCommandPool (const VulkanCommandPool&) = delete;
        VulkanCommandPool (VulkanCommandPool&&) noexcept;

        VulkanCommandPool& operator= (const VulkanCommandPool&) = delete;
        VulkanCommandPool& operator= (VulkanCommandPool&&) noexcept;

        ~VulkanCommandPool ();

        VulkanCommandBuffer getBuffer ();
        void reset ();

        void reserve (std::size_t capacity);
    };
}
