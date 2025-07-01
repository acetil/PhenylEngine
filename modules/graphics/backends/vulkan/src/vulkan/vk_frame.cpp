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
        m_syncs.push_back(FrameSync{
          .imageAvailable = VulkanSemaphore{resources},
          .inFlight = VulkanFence{resources, true},
        });
    }
}

bool FrameManager::onNewFrame (VulkanWindowFrameBuffer& windowFrameBuffer) {
    auto& sync = m_syncs[(m_frameCount + 1) % m_maxInFlight];

    PHENYL_ASSERT(sync.inFlight.wait(1000000000));

    auto frameImageOpt = windowFrameBuffer.acquireFrame(sync.imageAvailable);
    if (!frameImageOpt) {
        return false;
    }

    m_frameCount++;
    for (auto* i : m_listeners) {
        i->onFrameUpdate(flightNum());
    }
    // frameImage = *frameImageOpt;
    sync.inFlight.reset();

    getCommandPool().reset();

    return true;
}

void FrameManager::addFrameListener (IFrameListener* listener) {
    PHENYL_DASSERT(listener);
    m_listeners.emplace(listener);
    listener->onMaxFramesUpdate(m_maxInFlight);
    listener->onFrameUpdate(flightNum());
}

void FrameManager::removeFrameListener (IFrameListener* listener) {
    m_listeners.erase(listener);
}
