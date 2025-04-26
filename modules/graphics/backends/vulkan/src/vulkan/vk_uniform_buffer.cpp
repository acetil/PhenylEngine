#include "vk_renderer.h"

#include "vk_uniform_buffer.h"


using namespace phenyl::vulkan;

VulkanUniformBuffer::VulkanUniformBuffer (VulkanResources& resources) : resources{resources}, buffer{}, bufferInfo{} {

}

unsigned char* VulkanUniformBuffer::allocate (std::size_t size) {
    data = std::make_unique<std::byte[]>(size);
    this->size = size;
    buffer = VulkanBuffer{resources, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size};

    bufferInfo = VkDescriptorBufferInfo{
        .buffer = buffer.get(),
        .offset = 0,
        .range = static_cast<VkDeviceSize>(size)
    };

    return reinterpret_cast<unsigned char*>(data.get());
}

void VulkanUniformBuffer::upload () {
    PHENYL_ASSERT(buffer);
    PHENYL_DASSERT(data);

    buffer.copyIn(data.get(), size);
}

bool VulkanUniformBuffer::isReadable () const {
    return true;
}

std::size_t VulkanUniformBuffer::getMinAlignment () const noexcept {
    return resources.getDeviceProperties().minUniformAlignment;
}

VkBuffer VulkanUniformBuffer::getBuffer () const {
    PHENYL_ASSERT(buffer);
    return buffer.get();
}

VkDescriptorBufferInfo VulkanUniformBuffer::getBufferInfo (std::size_t offset, std::size_t size) const noexcept {
    PHENYL_ASSERT_MSG(offset + size <= this->size, "Attempted to bind uniform buffer out of bounds!");
    return VkDescriptorBufferInfo{
        .buffer = buffer.get(),
        .offset = static_cast<VkDeviceSize>(offset),
        .range = static_cast<VkDeviceSize>(size)
    };
}
