#pragma once

#include "vulkan_headers.h"
#include "init/vk_resources.h"

namespace phenyl::vulkan {
    class VulkanSemaphore {
    private:
        VulkanResource<VkSemaphore> m_sem;

    public:
        VulkanSemaphore (VulkanResources& resources);

        VkSemaphore get () const noexcept {
            return *m_sem;
        }

        VkSemaphoreSubmitInfo getSubmitInfo (VkPipelineStageFlags2 flags) const;
    };

    class VulkanFence {
    private:
        VkDevice m_device;
        VulkanResource<VkFence> m_fence;

    public:
        VulkanFence (VulkanResources& resources, bool signaled);

        bool wait (std::uint64_t timeout);
        void reset ();
        VkFence get () const noexcept {
            return *m_fence;
        }
    };
}
