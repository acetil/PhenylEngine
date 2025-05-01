#include "vk_sync.h"

using namespace phenyl::vulkan;

VulkanSemaphore::VulkanSemaphore (VulkanResources& resources) : m_sem{resources.makeSemaphore()} {
    PHENYL_ASSERT_MSG(m_sem, "Failed to create semaphore");
}


VkSemaphoreSubmitInfo VulkanSemaphore::getSubmitInfo (VkPipelineStageFlags2 flags) const {
    return VkSemaphoreSubmitInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = *m_sem,
        .value = 1,
        .stageMask = flags,
        .deviceIndex = 0,
    };
}

VulkanFence::VulkanFence (VulkanResources& resources, bool signaled) : m_device{resources.getDevice()} {
    VkFenceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    };

    if (signaled) {
        createInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;
    }

    m_fence = resources.makeFence(createInfo);
    PHENYL_ASSERT_MSG(m_fence, "Failed to create fence");
}

bool VulkanFence::wait (std::uint64_t timeout) {
    PHENYL_DASSERT(m_device);
    PHENYL_DASSERT(m_fence);

    auto vkFence = *m_fence;
    auto result = vkWaitForFences(m_device, 1, &vkFence, VK_TRUE, timeout);
    PHENYL_ASSERT_MSG(result == VK_SUCCESS || result == VK_TIMEOUT, "Fence wait failed: {}", result);

    return result == VK_SUCCESS;
}

void VulkanFence::reset () {
    PHENYL_DASSERT(m_device);
    PHENYL_DASSERT(m_fence);

    auto vkFence = *m_fence;
    vkResetFences(m_device, 1, &vkFence);
}
