#include "vk_transfer_manager.h"

using namespace phenyl::vulkan;

TransferManager::TransferManager (VulkanResources& resources) : m_queue{resources.getGraphicsQueue()}, m_commandPool{VulkanTransientCommandPool{resources}} {}

void TransferManager::queueTransfer (const std::function<void(VulkanCommandBuffer2& cmd)>& transferFunc) {
    auto cmd = m_commandPool.getBuffer();

    transferFunc(cmd);
    cmd.record();
    cmd.submit(nullptr, nullptr, nullptr); // TODO
}
