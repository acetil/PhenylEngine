#pragma once

#include "vulkan_headers.h"
#include "init/vk_resources.h"

namespace phenyl::vulkan {
    class VulkanSemaphore {
    private:
        VulkanResource<VkSemaphore> sem;

    public:
        VulkanSemaphore (VulkanResources& resources);

        VkSemaphore get () const noexcept {
            return *sem;
        }

        VkSemaphoreSubmitInfo getSubmitInfo (VkPipelineStageFlags2 flags) const;
    };

    class VulkanFence {
    private:
        VkDevice device;
        VulkanResource<VkFence> fence;

    public:
        VulkanFence (VulkanResources& resources, bool signaled);

        bool wait (std::uint64_t timeout);
        void reset ();
        VkFence get () const noexcept {
            return *fence;
        }
    };
}
