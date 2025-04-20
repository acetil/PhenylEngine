#include "vk_transfer_manager.h"

using namespace phenyl::vulkan;

TransferManager::TransferManager (VulkanResources& resources) : transferQueue{resources.getGraphicsQueue()}, commandPool{VulkanTransientCommandPool{resources}} {}

void TransferManager::queueTransfer (const std::function<void(VulkanCommandBuffer2& cmd)>& transferFunc) {
    auto cmd = commandPool.getBuffer();

    transferFunc(cmd);
    cmd.record();
    cmd.submit(nullptr, nullptr, nullptr); // TODO
}
