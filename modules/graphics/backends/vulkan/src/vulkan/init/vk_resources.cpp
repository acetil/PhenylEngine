#include "vk_resources.h"

#include "vk_device.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_RESOURCES", detail::VULKAN_LOGGER};

VulkanResources::VulkanResources (VkInstance instance, VulkanDevice& device, std::uint64_t maxFramesInFlight) :
    m_device{device},
    m_allocator{device.makeVmaAllocator(instance, VK_API_VERSION_1_3)},
    m_maxFramesInFlight{maxFramesInFlight},
    m_bufferQueue{[&] (VulkanBufferInfo info) {
        vmaDestroyBuffer(m_allocator, info.buffer, info.alloc);
    }},
    m_descriptorSetLayoutQueue{[&] (VkDescriptorSetLayout layout) {
        vkDestroyDescriptorSetLayout(device.device(), layout, nullptr);
    }},
    m_pipelineLayoutQueue{[&] (VkPipelineLayout layout) {
        vkDestroyPipelineLayout(device.device(), layout, nullptr);
    }},
    m_pipelineQueue{[&] (VkPipeline pipeline) {
        vkDestroyPipeline(device.device(), pipeline, nullptr);
    }},
    m_semaphoreQueue{[&] (VkSemaphore semaphore) {
        vkDestroySemaphore(device.device(), semaphore, nullptr);
    }},
    m_fenceQueue{[&] (VkFence fence) {
        vkDestroyFence(device.device(), fence, nullptr);
    }},
    m_commandPoolQueue{[&] (VkCommandPool pool) {
        vkDestroyCommandPool(device.device(), pool, nullptr);
    }},
    m_commandBufferQueue{[&] (VulkanCommandBufferInfo info) {
        vkFreeCommandBuffers(device.device(), info.pool, 1, &info.commandBuffer);
    }},
    m_imageQueue{[&] (VulkanImageInfo&& info) {
        vmaDestroyImage(m_allocator, info.image, info.alloc);
    }},
    m_imageViewQueue{[&] (VkImageView&& view) {
        vkDestroyImageView(device.device(), view, nullptr);
    }},
    m_samplerQueue{[&] (VkSampler&& sampler) {
        vkDestroySampler(device.device(), sampler, nullptr);
    }} {}

VulkanResources::~VulkanResources () {
    m_bufferQueue.clear();

    m_samplerQueue.clear();
    m_imageViewQueue.clear();
    m_imageQueue.clear();

    vmaDestroyAllocator(m_allocator);
}

VkDevice VulkanResources::getDevice () const noexcept {
    return m_device.device();
}

VkQueue VulkanResources::getGraphicsQueue () const noexcept {
    return m_device.graphicsQueue();
}

const DeviceProperties& VulkanResources::getDeviceProperties () const noexcept {
    return m_device.properties();
}

VulkanResource<VulkanBufferInfo> VulkanResources::makeBuffer (const VkBufferCreateInfo& createInfo,
    const VmaAllocationCreateInfo& allocCreateInfo) {
    VkBuffer buffer;
    VmaAllocation alloc;
    if (auto result = vmaCreateBuffer(m_allocator, &createInfo, &allocCreateInfo, &buffer, &alloc, nullptr);
        result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkBuffer using VMA: {}", result);
        return {};
    }

    PHENYL_DASSERT(buffer);
    PHENYL_DASSERT(alloc);

    return {VulkanBufferInfo{.allocator = m_allocator, .buffer = buffer, .alloc = alloc},
      [&] (VulkanBufferInfo&& info) {
          m_bufferQueue.queueDestruction(info, getDestructionFrame());
      }};
}

VulkanResource<VulkanImageInfo> VulkanResources::makeImage (const VkImageCreateInfo& createInfo,
    const VmaAllocationCreateInfo& allocCreateInfo) {
    VkImage image;
    VmaAllocation alloc;
    if (auto result = vmaCreateImage(m_allocator, &createInfo, &allocCreateInfo, &image, &alloc, nullptr);
        result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkImage using VMA: {}", result);
        return {};
    }

    PHENYL_DASSERT(image);
    PHENYL_DASSERT(alloc);

    return {VulkanImageInfo{.image = image, .alloc = alloc}, [&] (VulkanImageInfo&& info) {
                m_imageQueue.queueDestruction(info, getDestructionFrame());
            }};
}

VulkanResource<VkImageView> VulkanResources::makeImageView (const VkImageViewCreateInfo& createInfo) {
    VkImageView imageView;
    if (auto result = vkCreateImageView(m_device.device(), &createInfo, nullptr, &imageView); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkImageView: {}", result);
        return {};
    }
    PHENYL_DASSERT(imageView);

    return {imageView, [&] (auto&& view) {
                m_imageViewQueue.queueDestruction(view, getDestructionFrame());
            }};
}

VulkanResource<VkSampler> VulkanResources::makeSampler (const VkSamplerCreateInfo& createInfo) {
    VkSampler sampler;
    if (auto result = vkCreateSampler(m_device.device(), &createInfo, nullptr, &sampler); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkSampler: {}", result);
        return {};
    }
    PHENYL_DASSERT(sampler);

    return {sampler, [&] (auto&& s) {
                m_samplerQueue.queueDestruction(s, getDestructionFrame());
            }};
}

VulkanResource<VkDescriptorSetLayout> VulkanResources::makeDescriptorSetLayout (
    const VkDescriptorSetLayoutCreateInfo& layoutInfo) {
    VkDescriptorSetLayout descriptorSetLayout;
    if (auto result = vkCreateDescriptorSetLayout(m_device.device(), &layoutInfo, nullptr, &descriptorSetLayout);
        result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkDescriptorSetLayout: {}", result);
        return {};
    }
    PHENYL_DASSERT(descriptorSetLayout);

    return {descriptorSetLayout, [&] (VkDescriptorSetLayout&& layout) {
                m_descriptorSetLayoutQueue.queueDestruction(layout, getDestructionFrame());
            }};
}

VulkanResource<VkPipelineLayout> VulkanResources::makePipelineLayout (const VkPipelineLayoutCreateInfo& layoutInfo) {
    VkPipelineLayout pipelineLayout;
    if (auto result = vkCreatePipelineLayout(m_device.device(), &layoutInfo, nullptr, &pipelineLayout);
        result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkPipelineLayout: {}", result);
        return {};
    }
    PHENYL_DASSERT(pipelineLayout);

    return {pipelineLayout, [&] (VkPipelineLayout&& layout) {
                m_pipelineLayoutQueue.queueDestruction(layout, getDestructionFrame());
            }};
}

VulkanResource<VkPipeline> VulkanResources::makePipeline (const VkGraphicsPipelineCreateInfo& createInfo) {
    VkPipeline pipeline;
    if (auto result = vkCreateGraphicsPipelines(m_device.device(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
        result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkPipeline: {}", result);
        return {};
    }
    PHENYL_DASSERT(pipeline);

    return {pipeline, [&] (VkPipeline&& x) {
                m_pipelineQueue.queueDestruction(x, getDestructionFrame());
            }};
}

VulkanResource<VkCommandPool> VulkanResources::makeCommandPool (VkCommandPoolCreateFlags usage) {
    auto pool = m_device.makeCommandPool(usage);

    return pool ? VulkanResource<VkCommandPool>{pool,
                    [&] (VkCommandPool&& cmdPool) {
                        m_commandPoolQueue.queueDestruction(cmdPool, getDestructionFrame());
                    }} :
                  VulkanResource<VkCommandPool>{};
}

VulkanResource<VulkanCommandBufferInfo> VulkanResources::makeCommandBuffer (VkCommandPool pool,
    VkCommandBufferLevel level) {
    VkCommandBufferAllocateInfo allocInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .commandPool = pool,
      .level = level,
      .commandBufferCount = 1};

    VkCommandBuffer buffer;
    if (auto result = vkAllocateCommandBuffers(m_device.device(), &allocInfo, &buffer); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkCommandBuffer: {}", result);
        return {};
    }
    PHENYL_DASSERT(buffer);

    return {VulkanCommandBufferInfo{pool, buffer}, [&] (auto&& info) {
                m_commandBufferQueue.queueDestruction(info, getDestructionFrame());
            }};
}

VulkanResource<VkSemaphore> VulkanResources::makeSemaphore () {
    VkSemaphoreCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

    VkSemaphore semaphore;
    if (auto result = vkCreateSemaphore(m_device.device(), &createInfo, nullptr, &semaphore); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkSemaphore: {}", result);
        return {};
    }
    PHENYL_DASSERT(semaphore);

    return {semaphore, [&] (VkSemaphore&& sem) {
                m_semaphoreQueue.queueDestruction(sem, getDestructionFrame());
            }};
}

VulkanResource<VkFence> VulkanResources::makeFence (const VkFenceCreateInfo& createInfo) {
    VkFence fence;
    if (auto result = vkCreateFence(m_device.device(), &createInfo, nullptr, &fence); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to create VkFence: {}", result);
        return {};
    }
    PHENYL_DASSERT(fence);

    return {fence, [&] (VkFence&& f) {
                m_fenceQueue.queueDestruction(f, getDestructionFrame());
            }};
}

void VulkanResources::onFrame (std::uint64_t frameNum) {
    m_bufferQueue.onFrame(frameNum);

    m_samplerQueue.onFrame(frameNum);
    m_imageViewQueue.onFrame(frameNum);
    m_imageQueue.onFrame(frameNum);

    m_pipelineQueue.onFrame(frameNum);
    m_pipelineLayoutQueue.onFrame(frameNum);
    m_descriptorSetLayoutQueue.onFrame(frameNum);

    m_semaphoreQueue.onFrame(frameNum);
    m_fenceQueue.onFrame(frameNum);

    m_commandPoolQueue.onFrame(frameNum);

    this->m_frameNum = frameNum;
}

std::uint64_t VulkanResources::getDestructionFrame () const noexcept {
    return m_frameNum + m_maxFramesInFlight;
}
