#include "vk_sync.h"

using namespace phenyl::vulkan;

VulkanSemaphore::VulkanSemaphore (VulkanResources& resources) : sem{resources.makeSemaphore()} {
    PHENYL_ASSERT_MSG(sem, "Failed to create semaphore");
}


VkSemaphoreSubmitInfo VulkanSemaphore::getSubmitInfo (VkPipelineStageFlags2 flags) const {
    return VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = *sem,
        .value = 1,
        .stageMask = flags,
        .deviceIndex = 0,
    };
}

VulkanFence::VulkanFence (VulkanResources& resources, bool signaled) : device{resources.getDevice()} {
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };

    if (signaled) {
        createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
    }

    fence = resources.makeFence(createInfo);
    PHENYL_ASSERT_MSG(fence, "Failed to create fence");
}

bool VulkanFence::wait (std::uint64_t timeout) {
    PHENYL_DASSERT(device);
    PHENYL_DASSERT(fence);

    auto vkFence = *fence;
    auto result = vkWaitForFences(device, 1, &vkFence, VK_TRUE, timeout);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS || result == VK_TIMEOUT, "Fence wait failed: {}", result);

    return result == VK_SUCCESS;
}

void VulkanFence::reset () {
    PHENYL_DASSERT(device);
    PHENYL_DASSERT(fence);

    auto vkFence = *fence;
    vkResetFences(device, 1, &vkFence);
}
