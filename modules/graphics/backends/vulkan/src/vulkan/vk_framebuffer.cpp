#include "vk_framebuffer.h"

using namespace phenyl::vulkan;

void VulkanFrameBuffer::clear (glm::vec4 clearColor) {

}

const phenyl::graphics::ISampler* VulkanFrameBuffer::getSampler () const noexcept {
    return &dummy;
}

const phenyl::graphics::ISampler* VulkanFrameBuffer::getDepthSampler () const noexcept {
    return &dummy;
}

glm::ivec2 VulkanFrameBuffer::getDimensions () const noexcept {
    return {0, 0};
}
