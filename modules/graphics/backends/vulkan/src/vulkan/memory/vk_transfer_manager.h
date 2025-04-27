#pragma once

#include <functional>

#include "vulkan/vk_command_buffer.h"

namespace phenyl::vulkan {
    class TransferManager {
    private:
        VkQueue transferQueue;
        VulkanTransientCommandPool commandPool;
    public:
        TransferManager (VulkanResources& resources);

        void queueTransfer (const std::function<void(VulkanCommandBuffer2& cmd)>& transferFunc);
    };
}
