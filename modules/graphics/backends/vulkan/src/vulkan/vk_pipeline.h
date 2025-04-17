#pragma once

#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_uniform_buffer.h"
#include "graphics/backend/pipeline.h"

namespace phenyl::vulkan {
    struct TestFramebuffer {
        VulkanCommandBuffer2* renderingRecorder;
        VulkanDescriptorPool* descriptorPool;
        VkViewport viewport;
        VkRect2D scissor;
    };

    class VulkanStorageBuffer;

    class VulkanPipeline : public graphics::IPipeline {
    private:
        TestFramebuffer* testFramebuffer;
        VkDevice device;
        VulkanResource<VkPipeline> pipeline;
        VulkanResource<VkPipelineLayout> pipelineLayout;

        VulkanResource<VkDescriptorSetLayout> descriptorSetLayout;
        std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes;
        std::unordered_map<graphics::UniformBinding, const VkDescriptorBufferInfo*> boundUniformBuffers;

        std::vector<std::size_t> vertexBindingTypes;

        std::vector<const VulkanStorageBuffer*> boundVertexBuffers;
        std::vector<VkBuffer> boundVkBuffers;
        std::vector<VkDeviceSize> vertexBufferOffsets;

        const VulkanStorageBuffer* indexBuffer = nullptr;
        VkIndexType indexBufferType;

        VkDescriptorSet getDescriptorSet ();
    public:
        VulkanPipeline (VkDevice device, VulkanResource<VkPipeline> pipeline, VulkanResource<VkPipelineLayout> pipelineLayout, VulkanResource<VkDescriptorSetLayout> descriptorSetLayout,
            TestFramebuffer* framebuffer, std::vector<std::size_t> vertexBindingTypes, std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes);

        void bindBuffer (std::size_t type, graphics::BufferBinding binding, const graphics::IBuffer& buffer, std::size_t offset) override;
        void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override;
        void bindUniform (std::size_t type, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer) override;
        void bindSampler (graphics::SamplerBinding binding, const graphics::ISampler& sampler) override;

        void unbindIndexBuffer () override;

        void render (graphics::IFrameBuffer* fb, std::size_t vertices, std::size_t offset) override;
        void renderInstanced (graphics::IFrameBuffer* fb, std::size_t numInstances, std::size_t vertices, std::size_t offset) override;
    };

    class VulkanPipelineBuilder : public graphics::IPipelineBuilder {
    private:
        VulkanResources& resources;

        VkFormat colorFormat;

        TestFramebuffer* framebuffer;

        core::Asset<graphics::Shader> shader;

        std::vector<VkVertexInputBindingDescription> vertexBindings;
        std::vector<std::size_t> vertexBindingTypes;
        std::vector<VkVertexInputAttributeDescription> vertexAttribs;

        std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
        std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes;
        std::unordered_map<graphics::UniformBinding, const VulkanUniformBuffer*> boundUniforms;

        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE;

        VkPipelineColorBlendAttachmentState blendAttachment{
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

    public:
        VulkanPipelineBuilder (VulkanResources& resources, VkFormat swapChainFormat, TestFramebuffer* framebuffer);

        void withBlendMode (graphics::BlendMode mode) override;
        void withCullMode (graphics::CullMode mode) override;
        void withDepthMask (bool doMask) override;
        void withGeometryType (graphics::GeometryType type) override;

        void withShader (core::Asset<graphics::Shader> shader) override;

        graphics::BufferBinding withBuffer (std::size_t type, std::size_t size, graphics::BufferInputRate inputRate) override;
        void withAttrib (graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding, std::size_t offset) override;

        graphics::UniformBinding withUniform (std::size_t type, unsigned int location) override;
        graphics::SamplerBinding withSampler (unsigned int location) override;

        std::unique_ptr<graphics::IPipeline> build () override;
    };
}
