#include "vk_command_buffer.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_COMMAND_BUFFER", detail::VULKAN_LOGGER};

VulkanCommandPool::VulkanCommandPool (VulkanResources& resources, std::size_t capacity) : device{resources.getDevice()}, poolQueue{resources.getGraphicsQueue()}, pool{resources.makeCommandPool()} {
    PHENYL_ASSERT_MSG(pool, "Failed to create command pool!");

    reserve(capacity);
}

VulkanSingleUseCommandBuffer VulkanCommandPool::getBuffer () {
    if (availableBuffers.empty()) {
        addBuffers(1);
    }

    auto buffer = availableBuffers.back();
    PHENYL_DASSERT(buffer);

    availableBuffers.pop_back();
    usedBuffers.emplace_back(buffer);
    return VulkanSingleUseCommandBuffer{poolQueue, buffer};
}

void VulkanCommandPool::reset () {
    auto result = vkResetCommandPool(device, *pool, 0);
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
        .commandPool = *pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<std::uint32_t>(count)
    };

    if (auto result = vkAllocateCommandBuffers(device, &allocInfo, availableBuffers.data() + startIndex); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to allocate command buffer");
    }
}

VulkanTransientCommandPool::VulkanTransientCommandPool (VulkanResources& resources) : device{resources.getDevice()}, poolQueue{resources.getGraphicsQueue()}, pool{resources.makeCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)}, resources{&resources} {}

VulkanTransientCommandBuffer VulkanTransientCommandPool::getBuffer () {
    PHENYL_ASSERT(resources);
    PHENYL_ASSERT(pool);
    return VulkanTransientCommandBuffer{poolQueue, resources->makeCommandBuffer(*pool)};
}

VulkanCommandBuffer2::VulkanCommandBuffer2 (VkCommandBuffer commandBuffer, VkCommandBufferUsageFlags usage) : commandBuffer{commandBuffer} {
    PHENYL_ASSERT(commandBuffer);
    VkCommandBufferBeginInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usage
    };
    auto result = vkBeginCommandBuffer(commandBuffer, &info);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to begin command buffer: {}", result);
}

void VulkanCommandBuffer2::doImageTransition (VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
    PHENYL_TRACE(LOGGER, "Performing image transition: {} -> {}", oldLayout, newLayout);
    endRendering();

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

void VulkanCommandBuffer2::beginRendering (VkImageView imageView, VkImageLayout imageLayout, VkExtent2D drawExtent, std::optional<VkClearValue> clearColor) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot begin rendering for invalid buffer");

    RenderingInfo info{
        .imageView = imageView,
        .imageLayout = imageLayout,
        .drawExtent = drawExtent
    };

    if (currentRendering) {
        if (info == *currentRendering) {
            PHENYL_LOGW_IF(clearColor, LOGGER, "Ignoring clear value for duplicate rendering request");
            return;
        }

        endRendering();
    }

    PHENYL_DASSERT(!currentRendering);

    // TODO: move into framebuffer?
    VkRenderingAttachmentInfo colorAttachment{
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = imageView,
        .imageLayout = imageLayout,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .loadOp = clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE
    };
    if (clearColor) {
        colorAttachment.clearValue = *clearColor;
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
    currentRendering = info;
}

void VulkanCommandBuffer2::endRendering () {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot end rendering for invalid buffer");

    if (currentRendering) {
        vkCmdEndRendering(commandBuffer);
    }

    currentRendering = std::nullopt;
    currPipeline = nullptr;
}

void VulkanCommandBuffer2::setPipeline (VkPipeline pipeline, VkViewport viewport, VkRect2D scissor) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot set pipeline for invalid buffer");
    PHENYL_DASSERT(pipeline);

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

void VulkanCommandBuffer2::bindVertexBuffers (std::span<VkBuffer> buffers, std::span<VkDeviceSize> offsets) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot bind vertex buffers for invalid buffer");
    PHENYL_ASSERT(buffers.size() == offsets.size());

    vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<std::uint32_t>(buffers.size()), buffers.data(), offsets.data());
}

void VulkanCommandBuffer2::bindIndexBuffer (VkBuffer buffer, VkIndexType indexType) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot bind index buffer for invalid buffer");
    PHENYL_DASSERT(buffer);

    vkCmdBindIndexBuffer(commandBuffer, buffer, 0, indexType);
}

void VulkanCommandBuffer2::bindDescriptorSets (VkPipelineLayout pipelineLayout, std::span<VkDescriptorSet> descriptorSets) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot bind descriptor sets for invalid buffer");
    PHENYL_DASSERT(pipelineLayout);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<std::uint32_t>(descriptorSets.size()),
        descriptorSets.data(), 0, nullptr);
}

void VulkanCommandBuffer2::draw (std::uint32_t instanceCount, std::uint32_t vertexCount, std::uint32_t firstVertex) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot execute draw command for invalid buffer");
    vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, 0);
}

void VulkanCommandBuffer2::draw (std::uint32_t vertexCount, std::uint32_t firstVertex) {
    draw(1, vertexCount, firstVertex);
}

void VulkanCommandBuffer2::drawIndexed (std::uint32_t instanceCount, std::uint32_t indexCount, std::uint32_t firstIndex) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot execute draw command for invalid buffer");
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, 0, 0);
}

void VulkanCommandBuffer2::drawIndexed (std::uint32_t indexCount, std::uint32_t firstIndex) {
    drawIndexed(1, indexCount, firstIndex);
}

VulkanSingleUseCommandBuffer::VulkanSingleUseCommandBuffer (VkQueue bufferQueue, VkCommandBuffer commandBuffer) :
        VulkanCommandBuffer2{commandBuffer, 0}, bufferQueue{bufferQueue} {
    PHENYL_ASSERT(bufferQueue);
}

void VulkanSingleUseCommandBuffer::submit (const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem, const VulkanFence* fence) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot submit invalid command buffer");
    endRendering();

    if (auto result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to record command buffer, error: {}", result);
    }

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

    auto result = vkQueueSubmit2(bufferQueue, 1, &submitInfo, fence? fence->get() : VK_NULL_HANDLE);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to submit to queue: {}", result);

    commandBuffer = nullptr;
}

VulkanTransientCommandBuffer::VulkanTransientCommandBuffer (VkQueue bufferQueue, VulkanResource<VulkanCommandBufferInfo> cbInfo) :
        VulkanCommandBuffer2{cbInfo->commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT}, bufferQueue{bufferQueue}, cbInfo{std::move(cbInfo)} {
    PHENYL_ASSERT(bufferQueue);
    PHENYL_ASSERT(cbInfo);
}

void VulkanTransientCommandBuffer::record () {
    PHENYL_ASSERT_MSG(!recorded, "Cannot re-record command buffer!");
    PHENYL_ASSERT_MSG(commandBuffer, "cannot record invalid command buffer!");

    if (auto result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS) {
        PHENYL_ABORT("Failed to record command buffer, error: {}", result);
    }

    commandBuffer = nullptr;
    recorded = true;
}

void VulkanTransientCommandBuffer::submit (const VulkanSemaphore* waitSem, const VulkanSemaphore* signalSem,
    const VulkanFence* fence) {
    PHENYL_ASSERT_MSG(cbInfo, "Cannot submit invalid command buffer");
    PHENYL_ASSERT_MSG(recorded, "Cannot submit unrecorded command buffer");

    VkCommandBufferSubmitInfo cbSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = cbInfo->commandBuffer
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

    auto result = vkQueueSubmit2(bufferQueue, 1, &submitInfo, fence? fence->get() : VK_NULL_HANDLE);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to submit to queue: {}", result);
}

