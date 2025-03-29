#pragma once

#include "graphics/backend/buffer.h"

namespace phenyl::vulkan {
    class VulkanBuffer : public graphics::IBuffer {
    public:
        void upload (unsigned char* data, std::size_t size) override;
    };
}
