#include "vk_resources.h"

#include "vk_device.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_RESOURCES", detail::VULKAN_LOGGER};

VulkanResources::VulkanResources (VkInstance instance, VulkanDevice& device, std::uint64_t maxFramesInFlight) : device{device},
        allocator{device.makeVmaAllocator(instance, VK_API_VERSION_1_3)}, maxFramesInFlight{maxFramesInFlight},
        bufferQueue{[&] (VulkanBufferInfo info) { vmaDestroyBuffer(allocator, info.buffer, info.alloc); }},
        descriptorSetLayoutQueue{[&] (VkDescriptorSetLayout layout) { vkDestroyDescriptorSetLayout(device.device(), layout, nullptr); }},
        pipelineLayoutQueue{[&] (VkPipelineLayout layout) { vkDestroyPipelineLayout(device.device(), layout, nullptr); }},
        pipelineQueue{[&] (VkPipeline pipeline) { vkDestroyPipeline(device.device(), pipeline, nullptr); }},
        semaphoreQueue{[&] (VkSemaphore semaphore) { vkDestroySemaphore(device.device(), semaphore, nullptr); }},
        fenceQueue{[&] (VkFence fence) { vkDestroyFence(device.device(), fence, nullptr); }},
        commandPoolQueue{[&] (VkCommandPool pool) { vkDestroyCommandPool(device.device(), pool, nullptr); } },
        commandBufferQueue{[&] (VulkanCommandBufferInfo info) { vkFreeCommandBuffers(device.device(), info.pool, 1, &info.commandBuffer); } } {}

VulkanResources::~VulkanResources () {
    bufferQueue.clear();

    pipelineQueue.clear();
    pipelineLayoutQueue.clear();
    descriptorSetLayoutQueue.clear();

    semaphoreQueue.clear();
    fenceQueue.clear();

    vmaDestroyAllocator(allocator);
}

VkDevice VulkanResources::getDevice () const noexcept {
    return device.device();
}

VkQueue VulkanResources::getGraphicsQueue () const noexcept {
    return device.getGraphicsQueue();
}

VulkanResource<VulkanBufferInfo> VulkanResources::makeBuffer (const VkBufferCreateInfo& createInfo, const VmaAllocationCreateInfo& allocCreateInfo) {
    VkBuffer buffer;
    VmaAllocation alloc;
    if (auto result = vmaCreateBuffer(allocator, &createInfo, &allocCreateInfo, &buffer, &alloc, nullptr); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkBuffer using VMA: {}", result);
        return {};
    }

    PHENYL_DASSERT(buffer);
    PHENYL_DASSERT(alloc);

    return {VulkanBufferInfo{
        .allocator = allocator,
        .buffer = buffer,
        .alloc = alloc
    }, [&] (VulkanBufferInfo&& info) { bufferQueue.queueDestruction(info, getDestructionFrame()); }};
}

VulkanResource<VkDescriptorSetLayout> VulkanResources::makeDescriptorSetLayout (const VkDescriptorSetLayoutCreateInfo& layoutInfo) {
    VkDescriptorSetLayout descriptorSetLayout;
    if (auto result = vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &descriptorSetLayout); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkDescriptorSetLayout: {}", result);
        return {};
    }
    PHENYL_DASSERT(descriptorSetLayout);

    return {descriptorSetLayout,
        [&] (VkDescriptorSetLayout&& layout) { descriptorSetLayoutQueue.queueDestruction(layout, getDestructionFrame()); } };
}

VulkanResource<VkPipelineLayout> VulkanResources::makePipelineLayout (const VkPipelineLayoutCreateInfo& layoutInfo) {
    VkPipelineLayout pipelineLayout;
    if (auto result = vkCreatePipelineLayout(device.device(), &layoutInfo, nullptr, &pipelineLayout); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkPipelineLayout: {}", result);
        return {};
    }
    PHENYL_DASSERT(pipelineLayout);

    return {pipelineLayout, [&] (VkPipelineLayout&& layout) { pipelineLayoutQueue.queueDestruction(layout, getDestructionFrame()); }};
}

VulkanResource<VkPipeline> VulkanResources::makePipeline (const VkGraphicsPipelineCreateInfo& createInfo) {
    VkPipeline pipeline;
    if (auto result = vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkPipeline: {}", result);
        return {};
    }
    PHENYL_DASSERT(pipeline);

    return {pipeline, [&] (VkPipeline&& x) { pipelineQueue.queueDestruction(x, getDestructionFrame()); }};
}

VulkanResource<VkCommandPool> VulkanResources::makeCommandPool (VkCommandPoolCreateFlags usage) {
    auto pool = device.makeCommandPool(usage);

    return pool ? VulkanResource<VkCommandPool>{pool, [&] (VkCommandPool&& cmdPool) { commandPoolQueue.queueDestruction(cmdPool, getDestructionFrame()); }} : VulkanResource<VkCommandPool>{};
}

VulkanResource<VulkanCommandBufferInfo> VulkanResources::makeCommandBuffer (VkCommandPool pool, VkCommandBufferLevel level) {
    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = level,
        .commandBufferCount = 1
    };

    VkCommandBuffer buffer;
    if (auto result = vkAllocateCommandBuffers(device.device(), &allocInfo, &buffer); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkCommandBuffer: {}", result);
        return {};
    }
    PHENYL_DASSERT(buffer);

    return {VulkanCommandBufferInfo{pool, buffer}, [&] (auto&& info) { commandBufferQueue.queueDestruction(info, getDestructionFrame()); } };
}

VulkanResource<VkSemaphore> VulkanResources::makeSemaphore () {
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkSemaphore semaphore;
    if (auto result = vkCreateSemaphore(device.device(), &createInfo, nullptr, &semaphore); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkSemaphore: {}", result);
        return {};
    }
    PHENYL_DASSERT(semaphore);

    return {semaphore, [&] (VkSemaphore&& sem) { semaphoreQueue.queueDestruction(sem, getDestructionFrame()); }};
}

VulkanResource<VkFence> VulkanResources::makeFence (const VkFenceCreateInfo& createInfo) {
    VkFence fence;
    if (auto result = vkCreateFence(device.device(), &createInfo, nullptr, &fence); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkFence: {}", result);
        return {};
    }
    PHENYL_DASSERT(fence);

    return {fence, [&] (VkFence&& f) { fenceQueue.queueDestruction(f, getDestructionFrame()); }};
}

void VulkanResources::onFrame (std::uint64_t frameNum) {
    bufferQueue.onFrame(frameNum);

    pipelineQueue.onFrame(frameNum);
    pipelineLayoutQueue.onFrame(frameNum);
    descriptorSetLayoutQueue.onFrame(frameNum);

    semaphoreQueue.onFrame(frameNum);
    fenceQueue.onFrame(frameNum);

    commandPoolQueue.onFrame(frameNum);

    this->frameNum = frameNum;
}

std::uint64_t VulkanResources::getDestructionFrame () const noexcept {
    return frameNum + maxFramesInFlight;
}
