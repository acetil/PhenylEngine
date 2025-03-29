#pragma once

#include "graphics/backend/framebuffer.h"

namespace phenyl::vulkan {
    class VulkanFrameBuffer : public graphics::IFrameBuffer {
    private:
        class Sampler : public graphics::ISampler {
        public:
            std::size_t hash () const noexcept override {
                return 1;
            }
        };

        Sampler dummy{};
    public:
        void clear (glm::vec4 clearColor) override;

        const graphics::ISampler* getSampler () const noexcept override;

        const graphics::ISampler* getDepthSampler() const noexcept override;

        glm::ivec2 getDimensions() const noexcept override;
    };
}
