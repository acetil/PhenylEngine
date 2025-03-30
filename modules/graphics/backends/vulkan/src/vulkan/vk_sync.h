#pragma once

#include "vulkan_headers.h"

namespace phenyl::vulkan {
    class VulkanSemaphore {
    private:
        VkDevice device;
        VkSemaphore sem;

    public:
        VulkanSemaphore (VkDevice device);
        VulkanSemaphore (const VulkanSemaphore&) = delete;
        VulkanSemaphore (VulkanSemaphore&&) noexcept;

        VulkanSemaphore& operator= (const VulkanSemaphore&) = delete;
        VulkanSemaphore& operator= (VulkanSemaphore&&) noexcept;

        ~VulkanSemaphore ();

        VkSemaphore get () const noexcept {
            return sem;
        }

        VkSemaphoreSubmitInfo getSubmitInfo (VkPipelineStageFlags2 flags) const;
    };

    class VulkanFence {
    private:
        VkDevice device;
        VkFence fence;

    public:
        VulkanFence (VkDevice device, bool signaled);
        VulkanFence (const VulkanFence&) = delete;
        VulkanFence (VulkanFence&&) noexcept;

        VulkanFence& operator= (const VulkanFence&) = delete;
        VulkanFence& operator= (VulkanFence&&) noexcept;

        ~VulkanFence ();

        bool wait (std::uint64_t timeout);
        void reset ();
        VkFence get () const noexcept {
            return fence;
        }
    };
}
