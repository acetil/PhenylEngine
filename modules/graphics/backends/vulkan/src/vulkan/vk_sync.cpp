#include "vk_sync.h"

using namespace phenyl::vulkan;

VulkanSemaphore::VulkanSemaphore (VkDevice device) : device{device} {
    VkSemaphoreCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    VkSemaphore semaphore;
    auto result = vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create semaphore, error: {}", result);
    PHENYL_DASSERT(semaphore);

    sem = semaphore;
}

VulkanSemaphore::VulkanSemaphore (VulkanSemaphore&& other) noexcept : device{other.device}, sem{other.sem} {
    other.device = nullptr;
    other.sem = nullptr;
}

VulkanSemaphore& VulkanSemaphore::operator= (VulkanSemaphore&& other) noexcept {
    if (sem) {
        PHENYL_DASSERT(device);
        vkDestroySemaphore(device, sem, nullptr);
    }

    sem = other.sem;
    device = other.device;

    other.sem = nullptr;
    other.device = nullptr;

    return *this;
}

VulkanSemaphore::~VulkanSemaphore () {
    if (sem) {
        PHENYL_DASSERT(device);
        vkDestroySemaphore(device, sem, nullptr);
    }
}

VkSemaphoreSubmitInfo VulkanSemaphore::getSubmitInfo (VkPipelineStageFlags2 flags) const {
    return VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = sem,
        .value = 1,
        .stageMask = flags,
        .deviceIndex = 0,
    };
}

VulkanFence::VulkanFence (VkDevice device, bool signaled) : device{device} {
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };

    if (signaled) {
        createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VkFence fence;
    auto result = vkCreateFence(device, &createInfo, nullptr, &fence);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS, "Failed to create fence, error: {}", result);
    PHENYL_DASSERT(fence);

    this->fence = fence;
}

VulkanFence::VulkanFence (VulkanFence&& other) noexcept : device{other.device}, fence{other.fence} {
    other.device = nullptr;
    other.fence = nullptr;
}

VulkanFence& VulkanFence::operator= (VulkanFence&& other) noexcept {
    if (fence) {
        PHENYL_DASSERT(device);
        vkDestroyFence(device, fence, nullptr);
    }

    fence = other.fence;
    device = other.device;

    other.fence = nullptr;
    other.device = nullptr;

    return *this;
}

VulkanFence::~VulkanFence () {
    if (fence) {
        PHENYL_DASSERT(device);
        vkDestroyFence(device, fence, nullptr);
    }
}

bool VulkanFence::wait (std::uint64_t timeout) {
    PHENYL_DASSERT(device);
    PHENYL_DASSERT(fence);

    VkFence vkFence = fence;
    auto result = vkWaitForFences(device, 1, &vkFence, VK_TRUE, timeout);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS || result == VK_TIMEOUT, "Fence wait failed: {}", result);

    return result == VK_SUCCESS;
}

void VulkanFence::reset () {
    PHENYL_DASSERT(device);
    PHENYL_DASSERT(fence);

    vkResetFences(device, 1, &fence);
}
