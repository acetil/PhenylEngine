#include "vk_image_texture.h"

using namespace phenyl::vulkan;

std::uint32_t VulkanImageTexture::width () const noexcept {
    return 128;
}

std::uint32_t VulkanImageTexture::height () const noexcept {
    return 128;
}

void VulkanImageTexture::upload (const graphics::Image& image) {

}

const phenyl::graphics::ISampler& VulkanImageTexture::sampler () const noexcept {
    return dummy;
}
