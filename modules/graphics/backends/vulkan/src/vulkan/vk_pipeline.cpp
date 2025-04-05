#include "vulkan_headers.h"

#include "vk_pipeline.h"

#include <ranges>

#include "vk_storage_buffer.h"
#include "shader/vk_shader.h"

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

static phenyl::Logger LOGGER{"VK_PIPELINE", phenyl::vulkan::detail::VULKAN_LOGGER};

static VkVertexInputAttributeDescription MakeAttrib (BufferBinding binding, unsigned int location, VkFormat format, std::size_t offset);

static VkIndexType GetIndexType (ShaderIndexType type) {
    switch (type) {
        case ShaderIndexType::USHORT:
            return VK_INDEX_TYPE_UINT16;
        case ShaderIndexType::UINT:
            return VK_INDEX_TYPE_UINT32;
        default:
            PHENYL_ABORT("Unexpected index buffer type: {}", static_cast<std::uint32_t>(type));
    }
}

VulkanPipeline::VulkanPipeline (VkDevice device, VkPipeline pipeline, VkPipelineLayout pipelineLayout, TestFramebuffer* framebuffer, std::vector<std::size_t> vertexBindingTypes) :
        device{device}, pipeline{pipeline}, pipelineLayout{pipelineLayout}, testFramebuffer{framebuffer}, vertexBindingTypes{std::move(vertexBindingTypes)}, boundVertexBuffers(this->vertexBindingTypes.size()),
        vertexBufferOffsets(this->vertexBindingTypes.size()) {
    PHENYL_ASSERT(testFramebuffer);
}

VulkanPipeline::~VulkanPipeline () {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

void VulkanPipeline::bindBuffer (std::size_t type, BufferBinding binding, const IBuffer& buffer,
                                 std::size_t offset) {
    std::size_t index = binding;
    const auto* storageBuffer = reinterpret_cast<const VulkanStorageBuffer*>(&buffer);
    PHENYL_ASSERT_MSG(index < vertexBindingTypes.size(), "Attempted to bind to invalid binding!");
    PHENYL_ASSERT_MSG(type == vertexBindingTypes[index], "Attempted to bind buffer with incorrect type!");

    boundVertexBuffers[index] = storageBuffer;
    vertexBufferOffsets[index] = static_cast<VkDeviceSize>(offset);
}

void VulkanPipeline::bindIndexBuffer (ShaderIndexType type, const IBuffer& buffer) {
    const auto* storageBuffer = reinterpret_cast<const VulkanStorageBuffer*>(&buffer);

    indexBuffer = storageBuffer;
    indexBufferType = GetIndexType(type);
    PHENYL_DASSERT(indexBuffer);
}

void VulkanPipeline::bindUniform (std::size_t type, graphics::UniformBinding binding,
    const graphics::IUniformBuffer& buffer) {

}

void VulkanPipeline::bindSampler (SamplerBinding binding, const ISampler& sampler) {

}

void VulkanPipeline::unbindIndexBuffer () {
    indexBuffer = nullptr;
    indexBufferType = VK_INDEX_TYPE_NONE_KHR;
}

void VulkanPipeline::render (IFrameBuffer* fb, std::size_t vertices, std::size_t offset) {
    PHENYL_ASSERT(testFramebuffer->renderingRecorder);
    auto& recorder= *testFramebuffer->renderingRecorder;

    recorder.setPipeline(pipeline, testFramebuffer->viewport, testFramebuffer->scissor);

    boundVkBuffers = boundVertexBuffers
        | std::views::transform(&VulkanStorageBuffer::getBuffer)
        | std::ranges::to<std::vector>();
    recorder.bindVertexBuffers(boundVkBuffers, vertexBufferOffsets);

    if (indexBuffer) {
        PHENYL_ASSERT(indexBuffer->getBuffer());
        recorder.bindIndexBuffer(indexBuffer->getBuffer(), indexBufferType);
        recorder.drawIndexed(vertices, offset);
    } else {
        recorder.draw(vertices, offset);
    }
}

void VulkanPipeline::renderInstanced (IFrameBuffer* fb, std::size_t numInstances, std::size_t vertices,
    std::size_t offset) {
    PHENYL_ASSERT(testFramebuffer->renderingRecorder);
    auto& recorder= *testFramebuffer->renderingRecorder;

    recorder.setPipeline(pipeline, testFramebuffer->viewport, testFramebuffer->scissor);

    boundVkBuffers = boundVertexBuffers
        | std::views::transform(&VulkanStorageBuffer::getBuffer)
        | std::ranges::to<std::vector>();
    recorder.bindVertexBuffers(boundVkBuffers, vertexBufferOffsets);

    if (indexBuffer) {
        PHENYL_ASSERT(indexBuffer->getBuffer());
        recorder.bindIndexBuffer(indexBuffer->getBuffer(), indexBufferType);
        recorder.drawIndexed(numInstances, vertices, offset);
    } else {
        recorder.draw(numInstances, vertices, offset);
    }
}

void VulkanPipeline::renderTest (VulkanRenderingRecorder& recorder, VkViewport viewport, VkRect2D scissor, std::size_t vertices) {
    recorder.setPipeline(pipeline, viewport, scissor);

    recorder.draw(vertices, 0);
}

VulkanPipelineBuilder::VulkanPipelineBuilder (VkDevice device, VkFormat swapChainFormat, TestFramebuffer* framebuffer) : device{device}, colorFormat{swapChainFormat}, framebuffer{framebuffer} {}

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

void VulkanPipelineBuilder::withCullMode (CullMode mode) {
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
    VkVertexInputRate vkInputRate;
    switch (inputRate) {
        case graphics::BufferInputRate::VERTEX:
            vkInputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            break;
        case graphics::BufferInputRate::INSTANCE:
            vkInputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
            break;
        default:
            PHENYL_ABORT("Unexpected BufferInputRate value: {}", static_cast<std::uint32_t>(inputRate));
    }

    auto binding = static_cast<std::uint32_t>(vertexBindings.size());
    vertexBindings.push_back(VkVertexInputBindingDescription{
        .binding = binding,
        .stride = static_cast<std::uint32_t>(size),
        .inputRate = vkInputRate
    });
    vertexBindingTypes.emplace_back(type);

    return graphics::BufferBinding{binding};
}

void VulkanPipelineBuilder::withAttrib (graphics::ShaderDataType type, unsigned int location, graphics::BufferBinding binding, std::size_t offset) {
    PHENYL_ASSERT(binding < vertexBindings.size());

    switch (type) {
        case graphics::ShaderDataType::FLOAT32:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32_SFLOAT, offset));
            break;
        case graphics::ShaderDataType::INT16:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R16_SINT, offset));
            break;
        case graphics::ShaderDataType::INT32:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32_SINT, offset));
            break;
        case graphics::ShaderDataType::VEC2F:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32_SFLOAT, offset));
            break;
        case graphics::ShaderDataType::VEC3F:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32_SFLOAT, offset));
            break;
        case graphics::ShaderDataType::VEC4F:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32A32_SFLOAT, offset));
            break;
        case graphics::ShaderDataType::MAT2F:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32_SFLOAT, offset));
            vertexAttribs.push_back(MakeAttrib(binding, location + 1, VK_FORMAT_R32G32_SFLOAT, offset + sizeof(glm::vec2)));
            break;
        case graphics::ShaderDataType::MAT3F:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32_SFLOAT, offset));
            vertexAttribs.push_back(MakeAttrib(binding, location + 1, VK_FORMAT_R32G32B32_SFLOAT, offset + sizeof(glm::vec2)));
            vertexAttribs.push_back(MakeAttrib(binding, location + 2, VK_FORMAT_R32G32B32_SFLOAT, offset + sizeof(glm::vec2) * 2));
            break;
        case graphics::ShaderDataType::MAT4F:
            vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32A32_SFLOAT, offset));
            vertexAttribs.push_back(MakeAttrib(binding, location + 1, VK_FORMAT_R32G32B32A32_SFLOAT, offset + sizeof(glm::vec2)));
            vertexAttribs.push_back(MakeAttrib(binding, location + 2, VK_FORMAT_R32G32B32A32_SFLOAT, offset + sizeof(glm::vec2) * 2));
            vertexAttribs.push_back(MakeAttrib(binding, location + 3, VK_FORMAT_R32G32B32A32_SFLOAT, offset + sizeof(glm::vec2) * 3));
            break;
        default:
            PHENYL_LOGE(LOGGER, "Unexpected shader type: {}", static_cast<std::uint32_t>(type));
    }
}

phenyl::graphics::UniformBinding VulkanPipelineBuilder::withUniform (std::size_t type, unsigned int location) {
    return 0;
}

phenyl::graphics::SamplerBinding VulkanPipelineBuilder::withSampler (unsigned int location) {
    return 0;
}

std::unique_ptr<phenyl::graphics::IPipeline> VulkanPipelineBuilder::build () {
    VkPipelineVertexInputStateCreateInfo vertexInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast<std::uint32_t>(vertexBindings.size()),
        .pVertexBindingDescriptions = vertexBindings.data(),
        .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(vertexAttribs.size()),
        .pVertexAttributeDescriptions = vertexAttribs.data()
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
    return std::make_unique<VulkanPipeline>(device, pipeline, pipelineLayout, framebuffer, std::move(vertexBindingTypes));
}

static VkVertexInputAttributeDescription MakeAttrib (phenyl::graphics::BufferBinding binding, unsigned int location, VkFormat format, std::size_t offset) {
    return VkVertexInputAttributeDescription{
        .location = location,
        .binding = binding,
        .format = format,
        .offset = static_cast<std::uint32_t>(offset)
    };
}
