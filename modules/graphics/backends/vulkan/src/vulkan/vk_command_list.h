#pragma once
#include "graphics/backend/command_list.h"
#include "vk_command_buffer.h"

namespace phenyl::vulkan {
class CommandList : public graphics::ICommandList {
public:
    VulkanSingleUseCommandBuffer cmd;

    explicit CommandList (VulkanSingleUseCommandBuffer&& cmd) : cmd{std::move(cmd)} {}
};
} // namespace phenyl::vulkan
