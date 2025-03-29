#include "vk_uniform_buffer.h"

using namespace phenyl::vulkan;

unsigned char* VulkanUniformBuffer::allocate (std::size_t size) {
    data = std::make_unique<unsigned char[]>(size);
    return data.get();
}

void VulkanUniformBuffer::upload () {

}

bool VulkanUniformBuffer::isReadable () const {
    return false;
}
