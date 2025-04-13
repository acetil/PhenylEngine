#include "vk_renderer.h"

#include "vk_uniform_buffer.h"


using namespace phenyl::vulkan;

VulkanUniformBuffer::VulkanUniformBuffer (VulkanRenderer& renderer) : renderer{renderer}, buffer{}, bufferInfo{} {

}

unsigned char* VulkanUniformBuffer::allocate (std::size_t size) {
    data = std::make_unique<std::byte[]>(size);
    this->size = size;
    buffer = std::make_unique<VulkanBuffer>(renderer.makeBuffer(size, false));

    bufferInfo = VkDescriptorBufferInfo{
        .buffer = buffer->get(),
        .offset = 0,
        .range = static_cast<VkDeviceSize>(size)
    };

    return reinterpret_cast<unsigned char*>(data.get());
}

void VulkanUniformBuffer::upload () {
    PHENYL_ASSERT(buffer);
    PHENYL_DASSERT(data);

    buffer->copyIn(data.get(), size);
}

bool VulkanUniformBuffer::isReadable () const {
    return true;
}

VkBuffer VulkanUniformBuffer::getBuffer () const {
    PHENYL_ASSERT(buffer);
    return buffer->get();
}
