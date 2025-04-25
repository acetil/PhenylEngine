#include "vk_frame.h"

using namespace phenyl::vulkan;

FrameManager::FrameManager (VulkanDevice& device, VulkanResources& resources, std::size_t maxInFlight) : maxInFlight{maxInFlight} {
    PHENYL_DASSERT(maxInFlight > 0);

    commandPools.reserve(maxInFlight);
    descriptorPools.reserve(maxInFlight);
    syncs.reserve(maxInFlight);

    for (auto i = 0; i < maxInFlight; i++) {
        commandPools.emplace_back(resources);
        descriptorPools.emplace_back(device.device());
        syncs.push_back(FrameSync{
            .imageAvailable = VulkanSemaphore{resources},
            .renderFinished = VulkanSemaphore{resources},
            .inFlight = VulkanFence{resources, true}
        });
    }
}

bool FrameManager::onNewFrame (VulkanWindowFrameBuffer& windowFrameBuffer) {
    frameCount++;
    auto& sync = getFrameSync();

    PHENYL_ASSERT(sync.inFlight.wait(1000000000));

    auto frameImageOpt = windowFrameBuffer.acquireFrame(sync.imageAvailable);
    if (!frameImageOpt) {
        return false;
    }

    //frameImage = *frameImageOpt;
    sync.inFlight.reset();

    getCommandPool().reset();
    getDescriptorPool().reset();

    return true;
}
