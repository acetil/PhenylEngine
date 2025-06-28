#include "vk_frame.h"

using namespace phenyl::vulkan;

FrameManager::FrameManager (VulkanDevice& device, VulkanResources& resources, std::size_t maxInFlight) :
    m_maxInFlight{maxInFlight} {
    PHENYL_DASSERT(maxInFlight > 0);

    m_commandPools.reserve(maxInFlight);
    m_descriptorPools.reserve(maxInFlight);
    m_syncs.reserve(maxInFlight);

    for (auto i = 0; i < maxInFlight; i++) {
        m_commandPools.emplace_back(resources);
        m_descriptorPools.emplace_back(device.device());
        m_syncs.push_back(FrameSync{
          .imageAvailable = VulkanSemaphore{resources},
          .inFlight = VulkanFence{resources, true},
        });
    }
}

bool FrameManager::onNewFrame (VulkanWindowFrameBuffer& windowFrameBuffer) {
    m_frameCount++;
    auto& sync = getFrameSync();

    PHENYL_ASSERT(sync.inFlight.wait(1000000000));

    auto frameImageOpt = windowFrameBuffer.acquireFrame(sync.imageAvailable);
    if (!frameImageOpt) {
        return false;
    }

    // frameImage = *frameImageOpt;
    sync.inFlight.reset();

    getCommandPool().reset();
    getDescriptorPool().reset();

    return true;
}
