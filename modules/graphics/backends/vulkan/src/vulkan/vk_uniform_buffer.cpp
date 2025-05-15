#include "vk_uniform_buffer.h"

#include "vk_renderer.h"

using namespace phenyl::vulkan;

VulkanUniformBuffer::VulkanUniformBuffer (VulkanResources& resources) :
    m_resources{resources},
    m_buffer{},
    m_bufferInfo{} {}

std::span<std::byte> VulkanUniformBuffer::allocate (std::size_t size) {
    m_data = std::make_unique<std::byte[]>(size);
    this->m_size = size;
    m_buffer = VulkanBuffer{m_resources, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, size};

    m_bufferInfo =
        VkDescriptorBufferInfo{.buffer = m_buffer.get(), .offset = 0, .range = static_cast<VkDeviceSize>(size)};

    return {m_data.get(), size};
}

void VulkanUniformBuffer::upload () {
    PHENYL_ASSERT(m_buffer);
    PHENYL_DASSERT(m_data);

    m_buffer.copyIn({m_data.get(), m_size});
}

bool VulkanUniformBuffer::isReadable () const {
    return true;
}

std::size_t VulkanUniformBuffer::getMinAlignment () const noexcept {
    return m_resources.getDeviceProperties().minUniformAlignment;
}

VkBuffer VulkanUniformBuffer::getBuffer () const {
    PHENYL_ASSERT(m_buffer);
    return m_buffer.get();
}

VkDescriptorBufferInfo VulkanUniformBuffer::getBufferInfo (std::size_t offset, std::size_t size) const noexcept {
    PHENYL_ASSERT_MSG(offset + size <= this->m_size, "Attempted to bind uniform buffer out of bounds!");
    return VkDescriptorBufferInfo{.buffer = m_buffer.get(),
      .offset = static_cast<VkDeviceSize>(offset),
      .range = static_cast<VkDeviceSize>(size)};
}
