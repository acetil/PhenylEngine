#pragma once

#include "vk_sync.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanCommandBuffer2 {
    private:
        struct RenderingInfo {
            VkImageView imageView;
            VkImageLayout imageLayout;
            VkExtent2D drawExtent;

            bool operator== (const RenderingInfo&) const = default;
        };

        std::optional<RenderingInfo> currentRendering = std::nullopt;

        VkPipeline currPipeline = nullptr;
        std::optional<VkViewport> currViewport{};
        std::optional<VkRect2D> currScissor;

    protected:
        VkCommandBuffer commandBuffer;

        VulkanCommandBuffer2 (VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage);
    public:
        virtual ~VulkanCommandBuffer2 () = default;

        explicit operator bool () const noexcept {
            return commandBuffer;
        }

        void doImageTransition (VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

        void beginRendering (VkImageView imageView, VkImageLayout imageLayout, VkExtent2D drawExtent,
            std::optional<VkClearValue> clearColor = std::nullopt);
        void endRendering ();

        void setPipeline (VkPipeline pipeline, VkViewport viewport, VkRect2D scissor);

        void bindVertexBuffers (std::span<VkBuffer> buffers, std::span<VkDeviceSize> offsets);
        void bindIndexBuffer (VkBuffer buffer, VkIndexType indexType);

        void bindDescriptorSets (VkPipelineLayout pipelineLayout, std::span<VkDescriptorSet> descriptorSets);

        void draw (std::uint32_t instanceCount, std::uint32_t vertexCount, std::uint32_t firstVertex);
        void draw (std::uint32_t vertexCount, std::uint32_t firstVertex);

        void drawIndexed (std::uint32_t instanceCount, std::uint32_t indexCount, std::uint32_t firstIndex);
        void drawIndexed (std::uint32_t indexCount, std::uint32_t firstIndex);
    };

    class VulkanSingleUseCommandBuffer : public VulkanCommandBuffer2 {
    private:
        VkQueue bufferQueue;
    public:
        VulkanSingleUseCommandBuffer (VkQueue bufferQueue, VkCommandBuffer commandBuffer);

        void submit (const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence* fence);
    };

    class VulkanTransientCommandBuffer : public VulkanCommandBuffer2 {
    private:
        VkQueue bufferQueue;
        VulkanResource<VulkanCommandBufferInfo> cbInfo;
        bool recorded = false;
    public:
        VulkanTransientCommandBuffer (VkQueue bufferQueue, VulkanResource<VulkanCommandBufferInfo> cbInfo);

        void record ();
        void submit (const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence* fence);
    };

    class VulkanCommandPool {
    private:
        VkDevice device;
        VkQueue poolQueue;
        VulkanResource<VkCommandPool> pool;

        std::vector<VkCommandBuffer> availableBuffers;
        std::vector<VkCommandBuffer> usedBuffers;

        void addBuffers (std::size_t count);
    public:
        explicit VulkanCommandPool (VulkanResources& resources, std::size_t capacity = 1);

        VulkanSingleUseCommandBuffer getBuffer ();
        void reset ();

        void reserve (std::size_t capacity);
    };

    class VulkanTransientCommandPool {
    private:
        VkDevice device;
        VkQueue poolQueue;
        VulkanResources* resources;
        VulkanResource<VkCommandPool> pool;

    public:
        explicit VulkanTransientCommandPool (VulkanResources& resources);

        VulkanTransientCommandBuffer getBuffer ();
    };
}
