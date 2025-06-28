#pragma once

#include "init/vk_device.h"
#include "init/vk_swap_chain.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_framebuffer.h"
#include "vk_sync.h"
#include "vulkan_headers.h"

namespace phenyl::vulkan {
struct FrameSync {
    VulkanSemaphore imageAvailable;
    VulkanFence inFlight;
};

class FrameManager {
public:
    FrameManager (VulkanDevice& device, VulkanResources& resources, std::size_t maxInFlight);

    bool onNewFrame (VulkanWindowFrameBuffer& windowFrameBuffer);

    VulkanCommandPool& getCommandPool () {
        PHENYL_DASSERT(flightNum() < m_commandPools.size());
        return m_commandPools[flightNum()];
    }

    VulkanDescriptorPool& getDescriptorPool () {
        PHENYL_DASSERT(flightNum() < m_commandPools.size());
        return m_descriptorPools[flightNum()];
    }

    FrameSync& getFrameSync () {
        PHENYL_DASSERT(flightNum() < m_syncs.size());
        return m_syncs[flightNum()];
    }

    std::size_t frameNum () const noexcept {
        return m_frameCount;
    }

    std::size_t flightNum () const noexcept {
        return m_frameCount % m_maxInFlight;
    }

private:
    std::size_t m_frameCount = static_cast<std::size_t>(-1);
    std::size_t m_maxInFlight;

    std::vector<VulkanCommandPool> m_commandPools;
    std::vector<VulkanDescriptorPool> m_descriptorPools; // TODO: per-pipeline pools?
    std::vector<FrameSync> m_syncs;

    friend class VulkanRenderer;
};
} // namespace phenyl::vulkan
