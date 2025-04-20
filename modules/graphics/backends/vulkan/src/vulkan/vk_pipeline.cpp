#include "vulkan_headers.h"

#include "vk_pipeline.h"

#include <ranges>

#include "vk_storage_buffer.h"
#include "vk_uniform_buffer.h"
#include "shader/vk_shader.h"

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

static constexpr unsigned int MAX_ATTRIB_LOCATION = 64;
static constexpr unsigned int MAX_DESCRIPTOR_BINDING = 64;

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

VulkanPipeline::VulkanPipeline (VkDevice device, VulkanResource<VkPipeline> pipeline, VulkanResource<VkPipelineLayout> pipelineLayout, VulkanResource<VkDescriptorSetLayout> descriptorSetLayout,
                                TestFramebuffer* framebuffer, std::vector<std::size_t> vertexBindingTypes, std::unordered_map<graphics::UniformBinding, std::size_t> uniformTypes, std::unordered_set<graphics::SamplerBinding> validSamplers) :
        device{device}, pipeline{std::move(pipeline)}, pipelineLayout{std::move(pipelineLayout)}, descriptorSetLayout{std::move(descriptorSetLayout)}, testFramebuffer{framebuffer}, vertexBindingTypes{std::move(vertexBindingTypes)},
        boundVertexBuffers(this->vertexBindingTypes.size()), vertexBufferOffsets(this->vertexBindingTypes.size()), uniformTypes{std::move(uniformTypes)}, validSamplerBindings{std::move(validSamplers)} {
    PHENYL_ASSERT(testFramebuffer);
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
    const auto& uniformBuffer = reinterpret_cast<const VulkanUniformBuffer&>(buffer);

    auto it = uniformTypes.find(binding);
    PHENYL_ASSERT_MSG(it != uniformTypes.end(), "Attempted to bind unknown uniform {}", binding);
    PHENYL_ASSERT_MSG(type == it->second, "Attempted to bind uniform to binding {} with incorrect type! (expected: {}, got: {})", binding, it->second, type);

    boundUniformBuffers[binding] = &uniformBuffer.getBufferInfo();
}

void VulkanPipeline::bindSampler (SamplerBinding binding, const ISampler& sampler) {
    const auto& combinedSampler = reinterpret_cast<const IVulkanCombinedSampler&>(sampler);

    PHENYL_ASSERT_MSG(validSamplerBindings.contains(binding), "Attempted to bind unknown sampler {}", binding);

    if (binding == 0) {
        PHENYL_LOGD(LOGGER, "Temporarily ignoring 0 binding!");
        return;
    }

    boundSamplers[binding] = &combinedSampler;
}

void VulkanPipeline::unbindIndexBuffer () {
    indexBuffer = nullptr;
    indexBufferType = VK_INDEX_TYPE_NONE_KHR;
}

void VulkanPipeline::render (IFrameBuffer* fb, std::size_t vertices, std::size_t offset) {
    PHENYL_ASSERT(testFramebuffer->renderingRecorder);
    auto& recorder= *testFramebuffer->renderingRecorder;

    recorder.setPipeline(*pipeline, testFramebuffer->viewport, testFramebuffer->scissor);

    boundVkBuffers = boundVertexBuffers
        | std::views::transform(&VulkanStorageBuffer::getBuffer)
        | std::ranges::to<std::vector>();
    recorder.bindVertexBuffers(boundVkBuffers, vertexBufferOffsets);

    auto descriptorSet = getDescriptorSet();
    auto sets = std::array{descriptorSet};
    recorder.bindDescriptorSets(*pipelineLayout, sets);

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

    recorder.setPipeline(*pipeline, testFramebuffer->viewport, testFramebuffer->scissor);

    boundVkBuffers = boundVertexBuffers
        | std::views::transform(&VulkanStorageBuffer::getBuffer)
        | std::ranges::to<std::vector>();
    recorder.bindVertexBuffers(boundVkBuffers, vertexBufferOffsets);

    auto descriptorSet = getDescriptorSet();
    auto sets = std::array{descriptorSet};
    recorder.bindDescriptorSets(*pipelineLayout, sets);

    if (indexBuffer) {
        PHENYL_ASSERT(indexBuffer->getBuffer());
        recorder.bindIndexBuffer(indexBuffer->getBuffer(), indexBufferType);
        recorder.drawIndexed(numInstances, vertices, offset);
    } else {
        recorder.draw(numInstances, vertices, offset);
    }
}

VkDescriptorSet VulkanPipeline::getDescriptorSet () {
    // TODO: only allocate when necessary
    // TODO: incremental updates
    PHENYL_DASSERT(testFramebuffer);
    PHENYL_DASSERT(testFramebuffer->descriptorPool);

    auto set = testFramebuffer->descriptorPool->makeDescriptorSet(*descriptorSetLayout);
    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(boundUniformBuffers.size());

    for (const auto [binding, bufferInfo] : boundUniformBuffers) {
        writes.push_back(VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = bufferInfo
        });
    }

    std::vector<VkDescriptorImageInfo> imageDescriptors;
    imageDescriptors.reserve(boundSamplers.size());
    for (const auto [binding, sampler] : boundSamplers) {
        imageDescriptors.push_back(sampler->getDescriptor());
        writes.push_back(VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = set,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageDescriptors.back()
        });
    }

    vkUpdateDescriptorSets(device, writes.size(), writes.data(), 0, nullptr);

    return set;
}

VulkanPipelineBuilder::VulkanPipelineBuilder (VulkanResources& resources, VkFormat swapChainFormat, TestFramebuffer* framebuffer) :
        resources{resources}, colorFormat{swapChainFormat}, framebuffer{framebuffer} {}

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
    PHENYL_ASSERT_MSG(location < MAX_ATTRIB_LOCATION, "Attrib location {} larger than defined maximum of {}!", location, MAX_ATTRIB_LOCATION);

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

UniformBinding VulkanPipelineBuilder::withUniform (std::size_t type, unsigned int location) {
    PHENYL_ASSERT_MSG(location < MAX_DESCRIPTOR_BINDING, "Uniform binding {} larger than defined maximum of {}!", location, MAX_DESCRIPTOR_BINDING);
    descriptorBindings.push_back(VkDescriptorSetLayoutBinding{
        .binding = location,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS // TODO
    });
    uniformTypes[UniformBinding{location}] = type;

    return UniformBinding{location};
}

SamplerBinding VulkanPipelineBuilder::withSampler (unsigned int location) {
    PHENYL_ASSERT_MSG(location < MAX_DESCRIPTOR_BINDING, "Sampler binding {} larger than defined maximum of {}!", location, MAX_DESCRIPTOR_BINDING);
    descriptorBindings.push_back(VkDescriptorSetLayoutBinding{
        .binding = location,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS // TODO
    });
    registeredSamplers.emplace(location);

    return SamplerBinding{location};
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

    // TODO: multiple sets
    VkDescriptorSetLayoutCreateInfo layoutInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<std::uint32_t>(descriptorBindings.size()),
        .pBindings = descriptorBindings.data()
    };
    auto descriptorSetLayout = resources.makeDescriptorSetLayout(layoutInfo);
    if (!descriptorSetLayout) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline");
        return nullptr;
    }


    VkDescriptorSetLayout dsLayout = *descriptorSetLayout;
    // TODO
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &dsLayout
    };

    auto pipelineLayout = resources.makePipelineLayout(pipelineLayoutInfo);
    if (!pipelineLayout) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline");
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
        .layout = *pipelineLayout,
    };

    auto pipeline = resources.makePipeline(pipelineInfo);
    if (!pipeline) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline");
        return nullptr;
    }

    PHENYL_LOGD(LOGGER, "Constructed pipeline");
    return std::make_unique<VulkanPipeline>(resources.getDevice(), std::move(pipeline), std::move(pipelineLayout), std::move(descriptorSetLayout), framebuffer, std::move(vertexBindingTypes), std::move(uniformTypes), std::move(registeredSamplers));
}

static VkVertexInputAttributeDescription MakeAttrib (phenyl::graphics::BufferBinding binding, unsigned int location, VkFormat format, std::size_t offset) {
    return VkVertexInputAttributeDescription{
        .location = location,
        .binding = binding,
        .format = format,
        .offset = static_cast<std::uint32_t>(offset)
    };
}
