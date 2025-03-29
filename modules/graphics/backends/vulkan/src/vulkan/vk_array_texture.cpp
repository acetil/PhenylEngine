#include "vk_array_texture.h"

using namespace phenyl::vulkan;

std::uint32_t VulkanArrayTexture::width () const noexcept {
    return 128;
}

std::uint32_t VulkanArrayTexture::height () const noexcept {
    return 128;
}

std::uint32_t VulkanArrayTexture::size () const noexcept {
    return texSize;
}

void VulkanArrayTexture::reserve (std::uint32_t capacity) {

}

std::uint32_t VulkanArrayTexture::append () {
    return static_cast<std::uint32_t>(texSize++);
}

void VulkanArrayTexture::upload (std::uint32_t index, const graphics::Image& image) {

}

const phenyl::graphics::ISampler& VulkanArrayTexture::sampler () const noexcept {
    return dummy;
}
