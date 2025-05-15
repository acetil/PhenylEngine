#pragma once

#include "vk_sync.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
class IVulkanFrameBuffer;

class VulkanCommandBuffer2 {
public:
    virtual ~VulkanCommandBuffer2 () = default;

    explicit operator bool () const noexcept {
        return m_commandBuffer;
    }

    void doImageTransition (VkImage image, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBuffer (VkBuffer srcBuffer, std::size_t srcOffset, VkBuffer dstBuffer, std::size_t dstOffset,
        std::size_t size);

    void copyImage (VkImage fromImage, VkImageLayout fromLayout, VkImage toImage, VkImageLayout toLayout,
        VkExtent3D imageExtent, std::uint32_t numLayers = 1);
    void copyBufferToImage (VkBuffer buffer, VkImage image, VkImageLayout layout, VkExtent3D imageExtent,
        std::uint32_t layer = 0);

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

protected:
    VkCommandBuffer m_commandBuffer;

    VulkanCommandBuffer2 (VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage);

private:
    IVulkanFrameBuffer* m_currFrameBuffer = nullptr;

    VkPipeline m_currPipeline = nullptr;
    std::optional<VkViewport> m_currViewport{};
    std::optional<VkRect2D> m_currScissor;
};

class VulkanSingleUseCommandBuffer : public VulkanCommandBuffer2 {
public:
    VulkanSingleUseCommandBuffer (VkQueue bufferQueue, VkCommandBuffer commandBuffer);

    void submit (const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence* fence);

private:
    VkQueue m_queue;
};

class VulkanTransientCommandBuffer : public VulkanCommandBuffer2 {
public:
    VulkanTransientCommandBuffer (VkQueue bufferQueue, VulkanResource<VulkanCommandBufferInfo> cbInfo);

    void record ();
    void submit (const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence* fence);

private:
    VkQueue m_queue;
    VulkanResource<VulkanCommandBufferInfo> m_info;
    bool m_recorded = false;
};

class VulkanCommandPool {
public:
    explicit VulkanCommandPool (VulkanResources& resources, std::size_t capacity = 1);

    VulkanSingleUseCommandBuffer getBuffer ();
    void reset ();

    void reserve (std::size_t capacity);

private:
    VkDevice m_device;
    VkQueue m_queue;
    VulkanResource<VkCommandPool> m_pool;

    std::vector<VkCommandBuffer> m_availableBuffers;
    std::vector<VkCommandBuffer> m_usedBuffers;

    void addBuffers (std::size_t count);
};

class VulkanTransientCommandPool {
public:
    explicit VulkanTransientCommandPool (VulkanResources& resources);

    VulkanTransientCommandBuffer getBuffer ();

private:
    VkDevice m_device;
    VkQueue m_poolQueue;
    VulkanResources* m_resources;
    VulkanResource<VkCommandPool> m_pool;
};
} // namespace phenyl::vulkan
