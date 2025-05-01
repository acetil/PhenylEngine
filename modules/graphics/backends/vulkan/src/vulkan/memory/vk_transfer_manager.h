#pragma once

#include <functional>

#include "vulkan/vk_command_buffer.h"

namespace phenyl::vulkan {
    class TransferManager {
    public:
        TransferManager (VulkanResources& resources);

        void queueTransfer (const std::function<void(VulkanCommandBuffer2& cmd)>& transferFunc);

    private:
        VkQueue m_queue;
        VulkanTransientCommandPool m_commandPool;
    };
}
