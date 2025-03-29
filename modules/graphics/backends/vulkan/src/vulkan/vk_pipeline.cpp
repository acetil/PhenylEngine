#include "vk_pipeline.h"

using namespace phenyl::vulkan;

void VulkanPipeline::bindBuffer (std::size_t type, graphics::BufferBinding binding, const graphics::IBuffer& buffer,
    std::size_t offset) {

}

void VulkanPipeline::bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) {

}

void VulkanPipeline::bindUniform (std::size_t type, graphics::UniformBinding binding,
    const graphics::IUniformBuffer& buffer) {

}

void VulkanPipeline::bindSampler (graphics::SamplerBinding binding, const graphics::ISampler& sampler) {

}

void VulkanPipeline::unbindIndexBuffer () {

}

void VulkanPipeline::render (graphics::IFrameBuffer* fb, std::size_t vertices, std::size_t offset) {

}

void VulkanPipeline::renderInstanced (graphics::IFrameBuffer* fb, std::size_t numInstances, std::size_t vertices,
    std::size_t offset) {

}

void VulkanPipelineBuilder::withBlendMode (graphics::BlendMode mode) {

}

void VulkanPipelineBuilder::withCullMode (graphics::CullMode mode) {

}

void VulkanPipelineBuilder::withDepthMask (bool doMask) {

}

void VulkanPipelineBuilder::withGeometryType (graphics::GeometryType type) {

}

void VulkanPipelineBuilder::withShader (core::Asset<graphics::Shader> shader) {

}

phenyl::graphics::BufferBinding VulkanPipelineBuilder::withBuffer (std::size_t type, std::size_t size,
    graphics::BufferInputRate inputRate) {
    return 0;
}

void VulkanPipelineBuilder::withAttrib (graphics::ShaderDataType type, unsigned int location,
    graphics::BufferBinding binding, std::size_t offset) {

}

phenyl::graphics::UniformBinding VulkanPipelineBuilder::withUniform (std::size_t type, unsigned int location) {
    return 0;
}

phenyl::graphics::SamplerBinding VulkanPipelineBuilder::withSampler (unsigned int location) {
    return 0;
}

std::unique_ptr<phenyl::graphics::IPipeline> VulkanPipelineBuilder::build () {
    return std::make_unique<VulkanPipeline>();
}
