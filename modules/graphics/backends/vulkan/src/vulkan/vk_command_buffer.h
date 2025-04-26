#pragma once

#include "vk_sync.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class IVulkanFrameBuffer;

    class VulkanCommandBuffer2 {
    private:
        IVulkanFrameBuffer* currentFrameBuffer = nullptr;

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

        void doImageTransition (VkImage image, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout);

        void copyImage (VkImage fromImage, VkImageLayout fromLayout, VkImage toImage, VkImageLayout toLayout, VkExtent3D imageExtent, std::uint32_t numLayers = 1);
        void copyBufferToImage (VkBuffer buffer, VkImage image, VkImageLayout layout, VkExtent3D imageExtent, std::uint32_t layer = 0);

        void beginRendering (IVulkanFrameBuffer& frameBuffer);
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
