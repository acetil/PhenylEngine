#pragma once

#include "graphics/backend/uniform_buffer.h"

namespace phenyl::vulkan {
    class VulkanUniformBuffer : public graphics::IUniformBuffer {
    private:
        std::unique_ptr<unsigned char[]> data;
    public:
        unsigned char* allocate (std::size_t size) override;
        void upload () override;
        bool isReadable () const override;
    };
}