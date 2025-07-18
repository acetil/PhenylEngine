#pragma once

#include "graphics/backend/pipeline.h"
#include "texture/vk_image.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_frame.h"
#include "vk_framebuffer.h"
#include "vk_uniform_buffer.h"

#include <unordered_set>

namespace phenyl::vulkan {
class IVulkanStorageBuffer;
class VulkanWindowFrameBuffer;

class VulkanPipelineFactory {
public:
    VulkanPipelineFactory (VulkanResources& resources, std::shared_ptr<graphics::Shader> shader,
        VulkanResource<VkPipelineLayout> pipelineLayout, std::vector<VkVertexInputBindingDescription> vertexBindings,
        std::vector<VkVertexInputAttributeDescription> vertexAttribs, VkPrimitiveTopology topology,
        VkCullModeFlags cullMode, const VkPipelineColorBlendAttachmentState& blendAttachment,
        const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo);

    VkPipeline get (const FrameBufferLayout* layout);

    VkPipelineLayout layout () const noexcept {
        return *m_layout;
    }

private:
    std::unordered_map<const FrameBufferLayout*, VulkanResource<VkPipeline>> m_pipelines;

    VulkanResources& m_resources;
    std::shared_ptr<graphics::Shader> m_shader;
    VulkanResource<VkPipelineLayout> m_layout;

    std::vector<VkVertexInputBindingDescription> m_vertexBindings;
    std::vector<VkVertexInputAttributeDescription> m_attribs;

    VkPipelineInputAssemblyStateCreateInfo m_inputAssemblyInfo{};
    VkPipelineRasterizationStateCreateInfo m_rasterizerInfo{};
    VkPipelineDepthStencilStateCreateInfo m_depthStencilInfo;

    VkPipelineColorBlendAttachmentState m_colorBlendState;
};

class VulkanPipeline : public graphics::IPipeline {
private:
public:
    VulkanPipeline (VkDevice device, std::unique_ptr<VulkanPipelineFactory> pipelineFactory,
        VulkanResource<VkDescriptorSetLayout> descriptorSetLayout,
        std::unique_ptr<FramePool<VulkanDescriptorPool>> descriptorPools, VulkanWindowFrameBuffer* windowFb,
        std::vector<meta::TypeIndex> vertexBindingTypes,
        std::unordered_map<graphics::UniformBinding, meta::TypeIndex> uniformTypes,
        std::unordered_set<graphics::SamplerBinding> validSamplers);

    void bindBuffer (meta::TypeIndex type, graphics::BufferBinding binding, const graphics::IBuffer& buffer,
        std::size_t offset) override;
    void bindIndexBuffer (graphics::ShaderIndexType type, const graphics::IBuffer& buffer) override;
    void bindUniform (meta::TypeIndex type, graphics::UniformBinding binding, const graphics::IUniformBuffer& buffer,
        std::size_t offset, std::size_t size) override;
    void bindSampler (graphics::SamplerBinding binding, graphics::ISampler& sampler) override;

    void unbindIndexBuffer () override;

    void render (graphics::ICommandList& list, graphics::IFrameBuffer* fb, std::size_t vertices,
        std::size_t offset) override;
    void renderInstanced (graphics::ICommandList& list, graphics::IFrameBuffer* fb, std::size_t numInstances,
        std::size_t vertices, std::size_t offset) override;

private:
    struct UniformBufferBinding {
        const VulkanUniformBuffer* buffer;
        std::size_t offset;
        std::size_t size;
    };

    VulkanWindowFrameBuffer* m_windowFrameBuffer;

    VkDevice m_device;
    std::unique_ptr<VulkanPipelineFactory> m_pipelineFactory;

    VulkanResource<VkDescriptorSetLayout> m_descriptorSetLayout;
    std::unique_ptr<FramePool<VulkanDescriptorPool>> m_descriptorPools;

    std::unordered_map<graphics::UniformBinding, meta::TypeIndex> m_uniformTypes;
    std::unordered_map<graphics::UniformBinding, UniformBufferBinding> m_boundUniformBuffers;
    std::unordered_set<graphics::SamplerBinding> m_validSamplerBindings;
    std::unordered_map<graphics::SamplerBinding, IVulkanCombinedSampler*> m_boundSamplers;

    std::vector<meta::TypeIndex> m_vertexBindingTypes;

    std::vector<const IVulkanStorageBuffer*> m_boundVertexBuffers;
    std::vector<VkBuffer> m_boundVkBuffers;
    std::vector<VkDeviceSize> m_vertexBufferOffsets;

    const IVulkanStorageBuffer* m_indexBuffer = nullptr;
    VkIndexType m_indexBufferType;

    void prepareRender (VulkanCommandBuffer2& cmd, IVulkanFrameBuffer& frameBuffer);
    VkDescriptorSet getDescriptorSet (VulkanCommandBuffer2& cmd);
};

class VulkanPipelineBuilder : public graphics::IPipelineBuilder {
public:
    VulkanPipelineBuilder (VulkanResources& resources, FrameManager& frameManager, VulkanWindowFrameBuffer* windowFb);

    void withBlendMode (graphics::BlendMode mode) override;
    void withCullMode (graphics::CullMode mode) override;
    void withDepthTesting (bool doDepthWrite) override;
    void withGeometryType (graphics::GeometryType type) override;

    void withShader (const std::shared_ptr<graphics::Shader>& shader) override;

    graphics::BufferBinding withBuffer (meta::TypeIndex type, std::size_t size,
        graphics::BufferInputRate inputRate) override;
    void withAttrib (graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding,
        std::size_t offset) override;

    graphics::UniformBinding withUniform (meta::TypeIndex type, unsigned int location) override;
    graphics::SamplerBinding withSampler (unsigned int location) override;

    std::unique_ptr<graphics::IPipeline> build () override;

private:
    VulkanResources& m_resources;
    FrameManager& m_frameManager;

    VulkanWindowFrameBuffer* m_windowFb;

    std::shared_ptr<graphics::Shader> m_shader;

    std::vector<VkVertexInputBindingDescription> m_vertexBindings;
    std::vector<meta::TypeIndex> m_vertexBindingTypes;
    std::vector<VkVertexInputAttributeDescription> m_vertexAttribs;

    std::vector<VkDescriptorSetLayoutBinding> m_descriptorBindings;

    std::unordered_map<graphics::UniformBinding, meta::TypeIndex> m_uniformTypes;
    std::unordered_set<graphics::SamplerBinding> m_registeredSamplers;

    VkPrimitiveTopology m_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkCullModeFlags m_cullMode = VK_CULL_MODE_NONE;

    VkPipelineColorBlendAttachmentState m_blendAttachment{
      .blendEnable = VK_FALSE,
      .colorWriteMask =
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    bool m_depthTest = false;
    bool m_depthWrite = true;
};
} // namespace phenyl::vulkan
