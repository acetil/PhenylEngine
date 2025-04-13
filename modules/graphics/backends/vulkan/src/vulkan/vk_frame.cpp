#include "vk_frame.h"

using namespace phenyl::vulkan;

FrameManager::FrameManager (VulkanDevice& device, std::size_t maxInFlight) : maxInFlight{maxInFlight} {
    PHENYL_DASSERT(maxInFlight > 0);

    commandPools.reserve(maxInFlight);
    descriptorPools.reserve(maxInFlight);
    syncs.reserve(maxInFlight);

    for (auto i = 0; i < maxInFlight; i++) {
        commandPools.push_back(device.makeCommandPool(1));
        descriptorPools.emplace_back(device.device());
        syncs.push_back(FrameSync{
            .imageAvailable = VulkanSemaphore{device.device()},
            .renderFinished = VulkanSemaphore{device.device()},
            .inFlight = VulkanFence{device.device(), true}
        });
    }
}

bool FrameManager::onNewFrame (VulkanSwapChain& swapChain) {
    frameCount++;
    auto& sync = getFrameSync();

    PHENYL_ASSERT(sync.inFlight.wait(1000000000));

    auto frameImageOpt = swapChain.acquireImage(sync.imageAvailable);
    if (!frameImageOpt) {
        return false;
    }

    frameImage = *frameImageOpt;
    sync.inFlight.reset();

    getCommandPool().reset();
    getDescriptorPool().reset();

    return true;
}
