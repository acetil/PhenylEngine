#include "vk_command_buffer.h"

#include "vk_framebuffer.h"

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

void VulkanCommandBuffer2::doImageTransition (VkImage image, VkImageAspectFlags aspect, VkImageLayout oldLayout, VkImageLayout newLayout) {
    PHENYL_ASSERT(commandBuffer);
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
            .aspectMask = aspect, // TODO
            .levelCount = VK_REMAINING_MIP_LEVELS,
            .layerCount = VK_REMAINING_ARRAY_LAYERS
        }
    };

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
        imageBarrier.srcStageMask = 0;
        imageBarrier.srcAccessMask = 0;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        // TODO
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL) {
        // TODO
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL) {
        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;
    } else {
        PHENYL_ABORT("Unsupported source layout: {}", oldLayout);
    }

    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL) {
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        // TODO
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL) {
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;
    } else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        // TODO
        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
    } else {
        PHENYL_ABORT("Unsupported destination layout: {}", newLayout);
    }

    VkDependencyInfo depInfo{
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &imageBarrier
    };

    vkCmdPipelineBarrier2(commandBuffer, &depInfo);
}

void VulkanCommandBuffer2::copyBuffer (VkBuffer srcBuffer, std::size_t srcOffset, VkBuffer dstBuffer, std::size_t dstOffset, std::size_t size) {
    PHENYL_ASSERT(commandBuffer);

    VkBufferCopy2 region{
        .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
        .srcOffset = static_cast<VkDeviceSize>(srcOffset),
        .dstOffset = static_cast<VkDeviceSize>(dstOffset),
        .size = static_cast<VkDeviceSize>(size)
    };

    VkCopyBufferInfo2 info{
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .srcBuffer = srcBuffer,
        .dstBuffer = dstBuffer,
        .regionCount = 1,
        .pRegions = &region
    };

    vkCmdCopyBuffer2(commandBuffer, &info);
}

void VulkanCommandBuffer2::copyImage (VkImage fromImage, VkImageLayout fromLayout, VkImage toImage, VkImageLayout toLayout,
                                      VkExtent3D imageExtent, std::uint32_t numLayers) {
    PHENYL_ASSERT(commandBuffer);
    VkImageCopy2 region{
        .sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
        .srcSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // TODO
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = numLayers
        },
        .srcOffset = {0, 0, 0},
        .dstSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // TODO
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = numLayers
        },
        .dstOffset = {0, 0, 0},
        .extent = imageExtent
    };

    VkCopyImageInfo2 copyInfo{
        .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
        .srcImage = fromImage,
        .srcImageLayout = fromLayout,
        .dstImage = toImage,
        .dstImageLayout = toLayout,
        .regionCount = 1,
        .pRegions = &region
    };
    vkCmdCopyImage2(commandBuffer, &copyInfo);
}

void VulkanCommandBuffer2::copyBufferToImage (VkBuffer buffer, VkImage image, VkImageLayout layout, VkExtent3D imageExtent, std::uint32_t layer) {
    PHENYL_ASSERT(commandBuffer);
    VkBufferImageCopy2 region{
        .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
        .imageSubresource = VkImageSubresourceLayers{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, // TODO
            .mipLevel = 0,
            .baseArrayLayer = layer,
            .layerCount = 1,
        },
        .imageOffset = VkOffset3D{0, 0, 0}, // TODO
        .imageExtent = imageExtent
    };

    VkCopyBufferToImageInfo2 imageInfo{
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
        .srcBuffer = buffer,
        .dstImage = image,
        .dstImageLayout = layout,
        .regionCount = 1,
        .pRegions = &region
    };

    vkCmdCopyBufferToImage2(commandBuffer, &imageInfo);
}

void VulkanCommandBuffer2::beginRendering (IVulkanFrameBuffer& frameBuffer) {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot begin rendering for invalid buffer");

    if (currentFrameBuffer == &frameBuffer) {
        return;
    } else if (currentFrameBuffer) {
        endRendering();
    }
    PHENYL_DASSERT(!currentFrameBuffer);

    frameBuffer.prepareRendering(*this);
    vkCmdBeginRendering(commandBuffer, frameBuffer.getRenderingInfo());
    currentFrameBuffer = &frameBuffer;
}

void VulkanCommandBuffer2::endRendering () {
    PHENYL_ASSERT_MSG(commandBuffer, "Cannot end rendering for invalid buffer");

    if (currentFrameBuffer) {
        vkCmdEndRendering(commandBuffer);
    }

    currentFrameBuffer = nullptr;
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
    PHENYL_ASSERT(this->cbInfo);
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

