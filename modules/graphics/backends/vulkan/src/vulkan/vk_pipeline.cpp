#include "vulkan_headers.h"

#include "vk_pipeline.h"

#include "shader/vk_shader.h"

using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_PIPELINE", detail::VULKAN_LOGGER};

VulkanPipeline::VulkanPipeline (VkDevice device, VkPipeline pipeline, VkPipelineLayout pipelineLayout) : device{device}, pipeline{pipeline}, pipelineLayout{pipelineLayout} {}

VulkanPipeline::~VulkanPipeline () {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

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

void VulkanPipeline::renderTest (VulkanRenderingRecorder& recorder, VkViewport viewport, VkRect2D scissor, std::size_t vertices) {
    recorder.setPipeline(pipeline, viewport, scissor);

    recorder.draw(vertices, 0);
}

VulkanPipelineBuilder::VulkanPipelineBuilder (VkDevice device, VkFormat swapChainFormat) : device{device}, colorFormat{swapChainFormat} {}

void VulkanPipelineBuilder::withBlendMode (graphics::BlendMode mode) {
    switch (mode) {
        case graphics::BlendMode::ALPHA_BLEND:
            blendAttachment = VkPipelineColorBlendAttachmentState{
                .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
            };
            break;
        case graphics::BlendMode::ADDITIVE:
            blendAttachment = VkPipelineColorBlendAttachmentState{
                .blendEnable = VK_TRUE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
                .colorBlendOp = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp = VK_BLEND_OP_ADD,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
            };
            break;
    }
}

void VulkanPipelineBuilder::withCullMode (graphics::CullMode mode) {
    switch (mode) {
        case graphics::CullMode::FRONT_FACE:
            cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
        case graphics::CullMode::BACK_FACE:
            cullMode = VK_CULL_MODE_BACK_BIT;
            break;
        case graphics::CullMode::NONE:
            cullMode = VK_CULL_MODE_NONE;
            break;
    }
}

void VulkanPipelineBuilder::withDepthMask (bool doMask) {

}

void VulkanPipelineBuilder::withGeometryType (graphics::GeometryType type) {
    switch (type) {
        case graphics::GeometryType::TRIANGLES:
            topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case graphics::GeometryType::LINES:
            topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
    }
}

void VulkanPipelineBuilder::withShader (core::Asset<graphics::Shader> shader) {
    PHENYL_ASSERT(shader);
    this->shader = std::move(shader);
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
    // TODO
    VkPipelineVertexInputStateCreateInfo vertexInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = topology,
        .primitiveRestartEnable = VK_FALSE
    };

    std::array dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    VkPipelineViewportStateCreateInfo viewportStateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };

    VkPipelineRasterizationStateCreateInfo rasterizerInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE, // TODO
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = cullMode,
        .frontFace = VK_FRONT_FACE_CLOCKWISE, // TODO

        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f
    };


    VkPipelineMultisampleStateCreateInfo multisampleInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &blendAttachment
    };

    VkPipelineRenderingCreateInfo renderInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat = VK_FORMAT_UNDEFINED, // TODO
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
    };

    // TODO
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };

    VkPipelineLayout pipelineLayout;
    if (auto result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline layout, error: {}", result);
        return nullptr;
    }
    PHENYL_DASSERT(pipelineLayout);

    PHENYL_ASSERT(shader);
    auto& vkShader = reinterpret_cast<VulkanShader&>(shader->getUnderlying());
    auto shaderStages = vkShader.getStageInfo();

    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderInfo,
        .stageCount = static_cast<std::uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),

        .pVertexInputState = &vertexInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pViewportState = &viewportStateInfo,
        .pRasterizationState = &rasterizerInfo,
        .pMultisampleState = &multisampleInfo,
        .pDepthStencilState = nullptr, // TODO
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = pipelineLayout,
    };

    VkPipeline pipeline;
    if (auto result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline); result != VK_SUCCESS) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline, error: {}",  result);

        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        return nullptr;
    }

    PHENYL_LOGD(LOGGER, "Constructed pipeline");
    return std::make_unique<VulkanPipeline>(device, pipeline, pipelineLayout);
}
