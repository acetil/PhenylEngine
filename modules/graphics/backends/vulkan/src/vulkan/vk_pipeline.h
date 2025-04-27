#pragma once

#include <unordered_set>

#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_framebuffer.h"
#include "vk_uniform_buffer.h"
#include "graphics/backend/pipeline.h"
#include "texture/vk_image.h"

namespace phenyl::vulkan {
    struct TestFramebuffer {
        VulkanCommandBuffer2* renderingRecorder;
        VulkanDescriptorPool* descriptorPool;
    };

    class IVulkanStorageBuffer;
    class VulkanWindowFrameBuffer;

    class VulkanPipelineFactory {
    private:
        std::unordered_map<const FrameBufferLayout*, VulkanResource<VkPipeline>> pipelines;

        VulkanResources& resources;
        core::Asset<graphics::Shader> shader;
        VulkanResource<VkPipelineLayout> pipelineLayout;

        std::vector<VkVertexInputBindingDescription> vertexBindings;
        std::vector<VkVertexInputAttributeDescription> vertexAttribs;

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

        VkPipelineColorBlendAttachmentState colorBlendState;

    public:
        VulkanPipelineFactory (VulkanResources& resources, core::Asset<graphics::Shader> shader, VulkanResource<VkPipelineLayout> pipelineLayout, std::vector<VkVertexInputBindingDescription> vertexBindings,
            std::vector<VkVertexInputAttributeDescription> vertexAttribs, VkPrimitiveTopology topology, VkCullModeFlags cullMode, const VkPipelineColorBlendAttachmentState& blendAttachment,
            const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo);

        VkPipeline get (const FrameBufferLayout* layout);

        VkPipelineLayout layout () const noexcept {
            return *pipelineLayout;
        }
    };

    class VulkanPipeline : public graphics::IPipeline {
    private:
        struct UniformBufferBinding {
            const VulkanUniformBuffer* buffer;
            std::size_t offset;
            std::size_t size;
        };

        TestFramebuffer* testFramebuffer;
        VulkanWindowFrameBuffer* windowFrameBuffer;

        VkDevice device;
        std::unique_ptr<VulkanPipelineFactory> pipelineFactory;

        VulkanResource<VkDescriptorSetLayout> descriptorSetLayout;
        std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes;
        std::unordered_map<graphics::UniformBinding, UniformBufferBinding> boundUniformBuffers;
        std::unordered_set<graphics::SamplerBinding> validSamplerBindings;
        std::unordered_map<graphics::SamplerBinding, IVulkanCombinedSampler*> boundSamplers;

        std::vector<std::size_t> vertexBindingTypes;

        std::vector<const IVulkanStorageBuffer*> boundVertexBuffers;
        std::vector<VkBuffer> boundVkBuffers;
        std::vector<VkDeviceSize> vertexBufferOffsets;

        const IVulkanStorageBuffer* indexBuffer = nullptr;
        VkIndexType indexBufferType;

        void prepareRender (VulkanCommandBuffer2& cmd, IVulkanFrameBuffer& frameBuffer);
        VkDescriptorSet getDescriptorSet (VulkanCommandBuffer2& cmd);
    public:
        VulkanPipeline (VkDevice device, std::unique_ptr<VulkanPipelineFactory> pipelineFactory, VulkanResource<VkDescriptorSetLayout> descriptorSetLayout,
            TestFramebuffer* framebuffer, VulkanWindowFrameBuffer* windowFb, std::vector<std::size_t> vertexBindingTypes, std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes,
            std::unordered_set<graphics::SamplerBinding> validSamplers);

        void bindBuffer (std::size_t type, graphics::BufferBinding binding, const graphics::IBuffer& buffer, std::size_t offset) override;
        void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override;
        void bindUniform (std::size_t type, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer, std::size_t offset, std::size_t size) override;
        void bindSampler (graphics::SamplerBinding binding, graphics::ISampler& sampler) override;

        void unbindIndexBuffer () override;

        void render (graphics::IFrameBuffer* fb, std::size_t vertices, std::size_t offset) override;
        void renderInstanced (graphics::IFrameBuffer* fb, std::size_t numInstances, std::size_t vertices, std::size_t offset) override;
    };

    class VulkanPipelineBuilder : public graphics::IPipelineBuilder {
    private:
        VulkanResources& resources;

        VkFormat colorFormat;

        TestFramebuffer* framebuffer;
        VulkanWindowFrameBuffer* windowFb;

        core::Asset<graphics::Shader> shader;

        std::vector<VkVertexInputBindingDescription> vertexBindings;
        std::vector<std::size_t> vertexBindingTypes;
        std::vector<VkVertexInputAttributeDescription> vertexAttribs;

        std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;

        std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes;
        std::unordered_set<graphics::SamplerBinding> registeredSamplers;

        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkCullModeFlags cullMode = VK_CULL_MODE_NONE;

        VkPipelineColorBlendAttachmentState blendAttachment{
            .blendEnable = VK_FALSE,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        bool depthTest = false;
        bool depthWrite = true;

    public:
        VulkanPipelineBuilder (VulkanResources& resources, VkFormat swapChainFormat, TestFramebuffer* framebuffer, VulkanWindowFrameBuffer* windowFb);

        void withBlendMode (graphics::BlendMode mode) override;
        void withCullMode (graphics::CullMode mode) override;
        void withDepthTesting (bool doDepthWrite) override;
        void withGeometryType (graphics::GeometryType type) override;

        void withShader (core::Asset<graphics::Shader> shader) override;

        graphics::BufferBinding withBuffer (std::size_t type, std::size_t size, graphics::BufferInputRate inputRate) override;
        void withAttrib (graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding, std::size_t offset) override;

        graphics::UniformBinding withUniform (std::size_t type, unsigned int location) override;
        graphics::SamplerBinding withSampler (unsigned int location) override;

        std::unique_ptr<graphics::IPipeline> build () override;
    };
}
