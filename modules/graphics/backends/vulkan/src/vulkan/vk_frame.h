#pragma once

#include "vulkan_headers.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_framebuffer.h"
#include "vk_sync.h"
#include "init/vk_device.h"
#include "init/vk_swap_chain.h"

namespace phenyl::vulkan {
    struct FrameSync {
        VulkanSemaphore imageAvailable;
        VulkanSemaphore renderFinished;
        VulkanFence inFlight;
    };

    class FrameManager {
    private:
        std::size_t frameCount = static_cast<std::size_t>(-1);
        std::size_t maxInFlight;

        //SwapChainImage frameImage{};
        std::vector<VulkanCommandPool> commandPools;
        std::vector<VulkanDescriptorPool> descriptorPools; // TODO: per-pipeline pools?
        std::vector<FrameSync> syncs;

        friend class VulkanRenderer;
    public:
        FrameManager (VulkanDevice& device, VulkanResources& resources, std::size_t maxInFlight);

        bool onNewFrame (VulkanWindowFrameBuffer& windowFrameBuffer);

        // const SwapChainImage& getImage () const noexcept {
        //     return frameImage;
        // }

        VulkanCommandPool& getCommandPool () {
            PHENYL_DASSERT(flightNum() < commandPools.size());
            return commandPools[flightNum()];
        }

        VulkanDescriptorPool& getDescriptorPool () {
            PHENYL_DASSERT(flightNum() < commandPools.size());
            return descriptorPools[flightNum()];
        }

        FrameSync& getFrameSync () {
            PHENYL_DASSERT(flightNum() < syncs.size());
            return syncs[flightNum()];
        }

        std::size_t frameNum () const noexcept {
            return frameCount;
        }

        std::size_t flightNum () const noexcept {
            return frameCount % maxInFlight;
        }
    };
}
