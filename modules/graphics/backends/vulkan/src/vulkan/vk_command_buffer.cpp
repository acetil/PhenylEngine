#include "vk_command_buffer.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_COMMAND_BUFFER", detail::VULKAN_LOGGER};

VulkanCommandPool::VulkanCommandPool (VkDevice device, std::uint32_t queueIndex, std::size_t capacity) : device{device} {
    VkCommandPoolCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = queueIndex
    };
    VkCommandPool pool;
    if (auto result = vkCreateCommandPool(device, &createInfo, nullptr, &pool); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to create command pool, error: {}", result);
    }
    PHENYL_DASSERT(pool);

    commandPool = pool;

    reserve(capacity);
}

VulkanCommandPool::VulkanCommandPool (VulkanCommandPool&& other) noexcept : device{other.device}, commandPool{other.commandPool},
        availableBuffers{std::move(other.availableBuffers)}, usedBuffers{std::move(other.usedBuffers)} {
    other.device = nullptr;
    other.commandPool = nullptr;
}

VulkanCommandPool& VulkanCommandPool::operator= (VulkanCommandPool&& other) noexcept {
    if (commandPool) {
        PHENYL_DASSERT(device);
        vkDestroyCommandPool(device, commandPool, nullptr);
    }

    device = other.device;
    commandPool = other.commandPool;
    availableBuffers = std::move(other.availableBuffers);
    usedBuffers = std::move(other.usedBuffers);

    other.device = nullptr;
    other.commandPool = nullptr;

    return *this;
}

VulkanCommandPool::~VulkanCommandPool () {
    if (commandPool) {
        PHENYL_DASSERT(device);
        vkDestroyCommandPool(device, commandPool, nullptr);
    }
}

VulkanCommandBuffer VulkanCommandPool::getBuffer () {
    if (availableBuffers.empty()) {
        addBuffers(1);
    }

    auto buffer = availableBuffers.back();
    PHENYL_DASSERT(buffer);

    availableBuffers.pop_back();
    usedBuffers.emplace_back(buffer);
    return VulkanCommandBuffer{buffer};
}

void VulkanCommandPool::reset () {
    auto result = vkResetCommandPool(device, commandPool, 0);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to reset command pool: {}", result);

    std::ranges::copy(usedBuffers, std::back_inserter(availableBuffers));
    usedBuffers.clear();
}

void VulkanCommandPool::reserve (std::size_t capacity) {
    auto currCapacity = availableBuffers.size() + usedBuffers.size();
    if (currCapacity < capacity) {
        addBuffers(capacity - currCapacity);
    }
}

void VulkanCommandPool::addBuffers (std::size_t count) {
    auto startIndex = availableBuffers.size();
    availableBuffers.resize(availableBuffers.size() + count);

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<std::uint32_t>(count)
    };

    if (auto result = vkAllocateCommandBuffers(device, &allocInfo, availableBuffers.data() + startIndex); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to allocate command buffer");
    }
}

VulkanCommandBuffer::VulkanCommandBuffer (VkCommandBuffer commandBuffer) : commandBuffer{commandBuffer} {
    PHENYL_DASSERT(commandBuffer);

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

VulkanRenderingRecorder VulkanCommandBuffer::beginRendering (VkImageView imageView, VkImageLayout imageLayout, VkExtent2D drawExtent, std::optional<VkClearValue> clearValue) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot create recorder for invalid buffer");
    PHENYL_ASSERT_MSG(!inRecorder, "Cannot create recorder while previous recorder is active");

    // TODO: move into framebuffer?
    VkRenderingAttachmentInfo colorAttachment{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = imageView,
        .imageLayout = imageLayout,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .loadOp = clearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE
    };
    if (clearValue) {
        colorAttachment.clearValue = *clearValue;
    }

    VkRenderingInfo renderingInfo{
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea = {
            .offset = {0, 0},
            .extent = drawExtent
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        // TODO: depth/stencil
    };

    vkCmdBeginRendering(commandBuffer, &renderingInfo);
    inRecorder = true;
    return VulkanRenderingRecorder{*this, commandBuffer};
}

void VulkanCommandBuffer::endRendering () {
    PHENYL_DASSERT(inRecorder);
    vkCmdEndRendering(commandBuffer);
    inRecorder = false;
}

void VulkanCommandBuffer::doImageTransition (VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    PHENYL_ASSERT_MSG(!inRecorder, "Cannot do image transition while inside recorder!");

    VkImageMemoryBarrier2 imageBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .image = image,
        .subresourceRange = VkImageSubresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // TODO
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .layerCount = VK_REMAINING_ARRAY_LAYERS
        }
    };

    VkDependencyInfo depInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &imageBarrier
    };

    vkCmdPipelineBarrier2(commandBuffer, &depInfo);
}

VulkanRecordedCommandBuffer VulkanCommandBuffer::record () {
    PHENYL_ASSERT_MSG(!inRecorder, "Cannot record command buffer while in recorder");
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot record invalid buffer");

    if (auto result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to record command buffer, error: {}", result);
    }

    VulkanRecordedCommandBuffer recordedBuffer{commandBuffer};
    commandBuffer = nullptr;
    return recordedBuffer;
}

void VulkanCommandBuffer::drop () {
    PHENYL_ASSERT_MSG(!inRecorder, "Cannot drop command buffer while in recorder");

    auto result = vkEndCommandBuffer(commandBuffer);
    PHENYL_LOGW_IF(result != VK_SUCCESS, LOGGER, "Error while dropping command buffer: {}", result);

    commandBuffer = nullptr;
}

VulkanRenderingRecorder::VulkanRenderingRecorder (VulkanCommandBuffer& owner, VkCommandBuffer commandBuffer) : owner{owner}, commandBuffer{commandBuffer} {
    PHENYL_DASSERT(commandBuffer);
}

void VulkanRenderingRecorder::setPipeline (VkPipeline pipeline, VkViewport viewport, VkRect2D scissor) {
    PHENYL_DASSERT(commandBuffer);

    bool setViewScissor = false;
    if (pipeline != currPipeline) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        currPipeline = pipeline;
        setViewScissor = true;
    }

    if (setViewScissor || !currViewport || viewport != *currViewport) {
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        currViewport = viewport;
    }

    if (setViewScissor || !currScissor || scissor != *currScissor) {
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        currScissor = scissor;
    }
}

void VulkanRenderingRecorder::draw (std::uint32_t instanceCount, std::uint32_t vertexCount, std::uint32_t firstVertex) {
    PHENYL_DASSERT(commandBuffer);
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, 0);
}

void VulkanRenderingRecorder::draw (std::uint32_t vertexCount, std::uint32_t firstVertex) {
    draw(1, vertexCount, firstVertex);
}

void VulkanRenderingRecorder::drawIndexed (std::uint32_t instanceCount, std::uint32_t indexCount,std::uint32_t firstIndex) {
    PHENYL_DASSERT(commandBuffer);
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, 0, 0);
}

void VulkanRenderingRecorder::drawIndexed (std::uint32_t indexCount, std::uint32_t firstIndex) {
    drawIndexed(1, indexCount, firstIndex);
}

void VulkanRenderingRecorder::bindVertexBuffers (std::span<VkBuffer> buffers, std::span<VkDeviceSize> offsets) {
    PHENYL_DASSERT(commandBuffer);
    PHENYL_DASSERT(buffers.size() == offsets.size());

    vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<std::uint32_t>(buffers.size()), buffers.data(), offsets.data());
}

void VulkanRenderingRecorder::bindIndexBuffer (VkBuffer buffer, VkIndexType indexType) {
    PHENYL_DASSERT(commandBuffer);
    PHENYL_DASSERT(buffer);

    vkCmdBindIndexBuffer(commandBuffer, buffer, 0, indexType);
}

VulkanRenderingRecorder::~VulkanRenderingRecorder () {
    owner.endRendering();
}

VulkanRecordedCommandBuffer::VulkanRecordedCommandBuffer (VkCommandBuffer commandBuffer) : commandBuffer{commandBuffer} {
    PHENYL_DASSERT(commandBuffer);
}

void VulkanRecordedCommandBuffer::submit (VkQueue queue, const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence& fence) {
    VkCommandBufferSubmitInfo cbSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = commandBuffer
    };

    auto waitInfo = waitSem ? waitSem->getSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR) : VkSemaphoreSubmitInfo{};
    auto signalInfo = signalSem ? signalSem->getSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR) : VkSemaphoreSubmitInfo{};

    VkSubmitInfo2 submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = waitSem ? 1u : 0u,
        .pWaitSemaphoreInfos = &waitInfo,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cbSubmitInfo,
        .signalSemaphoreInfoCount = signalSem ? 1u : 0u,
        .pSignalSemaphoreInfos = &signalInfo,
    };

    auto result = vkQueueSubmit2(queue, 1, &submitInfo, fence.get());
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to submit to queue: {}", result);
}
