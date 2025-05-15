#pragma once

#include "init/vk_resources.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
class VulkanSemaphore {
public:
    VulkanSemaphore (VulkanResources& resources);

    VkSemaphore get () const noexcept {
        return *m_sem;
    }

    VkSemaphoreSubmitInfo getSubmitInfo (VkPipelineStageFlags2 flags) const;

private:
    VulkanResource<VkSemaphore> m_sem;
};

class VulkanFence {
public:
    VulkanFence (VulkanResources& resources, bool signaled);

    bool wait (std::uint64_t timeout);
    void reset ();

    VkFence get () const noexcept {
        return *m_fence;
    }

private:
    VkDevice m_device;
    VulkanResource<VkFence> m_fence;
};
} // namespace phenyl::vulkan
