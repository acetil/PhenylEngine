#include "vk_pipeline.h"

#include "shader/vk_shader.h"
#include "vk_framebuffer.h"
#include "vk_storage_buffer.h"
#include "vk_uniform_buffer.h"
#include "vulkan_headers.h"

#include <ranges>

using namespace phenyl::graphics;
using namespace phenyl::vulkan;

static constexpr unsigned int MAX_ATTRIB_LOCATION = 64;
static constexpr unsigned int MAX_DESCRIPTOR_BINDING = 64;

static phenyl::Logger LOGGER{"VK_PIPELINE", phenyl::vulkan::detail::VULKAN_LOGGER};

static VkVertexInputAttributeDescription MakeAttrib (BufferBinding binding, unsigned int location, VkFormat format,
    std::size_t offset);

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

VulkanPipeline::VulkanPipeline (VkDevice device, std::unique_ptr<VulkanPipelineFactory> pipelineFactory,
    VulkanResource<VkDescriptorSetLayout> descriptorSetLayout,
    std::unique_ptr<FramePool<VulkanDescriptorPool>> descriptorPools, VulkanWindowFrameBuffer* windowFb,
    std::vector<meta::TypeIndex> vertexBindingTypes,
    std::unordered_map<graphics::UniformBinding, meta::TypeIndex> uniformTypes,
    std::unordered_set<graphics::SamplerBinding> validSamplers) :
    m_device{device},
    m_pipelineFactory{std::move(pipelineFactory)},
    m_descriptorSetLayout{std::move(descriptorSetLayout)},
    m_descriptorPools{std::move(descriptorPools)},
    m_windowFrameBuffer{windowFb},
    m_vertexBindingTypes{std::move(vertexBindingTypes)},
    m_boundVertexBuffers(this->m_vertexBindingTypes.size()),
    m_vertexBufferOffsets(this->m_vertexBindingTypes.size()),
    m_uniformTypes{std::move(uniformTypes)},
    m_validSamplerBindings{std::move(validSamplers)} {
    PHENYL_DASSERT(windowFb);
}

void VulkanPipeline::bindBuffer (meta::TypeIndex type, BufferBinding binding, const IBuffer& buffer,
    std::size_t offset) {
    std::size_t index = binding;
    const auto* storageBuffer = reinterpret_cast<const IVulkanStorageBuffer*>(&buffer);
    PHENYL_ASSERT_MSG(index < m_vertexBindingTypes.size(), "Attempted to bind to invalid binding!");
    PHENYL_ASSERT_MSG(type == m_vertexBindingTypes[index], "Attempted to bind buffer with incorrect type!");

    m_boundVertexBuffers[index] = storageBuffer;
    m_vertexBufferOffsets[index] = static_cast<VkDeviceSize>(offset);
}

void VulkanPipeline::bindIndexBuffer (ShaderIndexType type, const IBuffer& buffer) {
    const auto* storageBuffer = reinterpret_cast<const IVulkanStorageBuffer*>(&buffer);

    m_indexBuffer = storageBuffer;
    m_indexBufferType = GetIndexType(type);
    PHENYL_DASSERT(m_indexBuffer);
}

void VulkanPipeline::bindUniform (meta::TypeIndex type, graphics::UniformBinding binding,
    const graphics::IUniformBuffer& buffer, std::size_t offset, std::size_t size) {
    const auto& uniformBuffer = reinterpret_cast<const VulkanUniformBuffer&>(buffer);

    auto it = m_uniformTypes.find(binding);
    PHENYL_ASSERT_MSG(it != m_uniformTypes.end(), "Attempted to bind unknown uniform {}", binding);
    PHENYL_ASSERT_MSG(type == it->second,
        "Attempted to bind uniform to binding {} with incorrect type! (expected: {}, got: {})", binding, it->second,
        type);

    m_boundUniformBuffers[binding] = UniformBufferBinding{
      .buffer = &uniformBuffer,
      .offset = offset,
      .size = size,
    };
}

void VulkanPipeline::bindSampler (SamplerBinding binding, ISampler& sampler) {
    PHENYL_ASSERT_MSG(m_validSamplerBindings.contains(binding), "Attempted to bind unknown sampler {}", binding);

    auto& combinedSampler = reinterpret_cast<IVulkanCombinedSampler&>(sampler);
    m_boundSamplers[binding] = &combinedSampler;
}

void VulkanPipeline::unbindIndexBuffer () {
    m_indexBuffer = nullptr;
    m_indexBufferType = VK_INDEX_TYPE_NONE_KHR;
}

void VulkanPipeline::render (graphics::ICommandList& list, IFrameBuffer* fb, std::size_t vertices, std::size_t offset) {
    renderInstanced(list, fb, 1, vertices, offset);
}

void VulkanPipeline::renderInstanced (graphics::ICommandList& list, IFrameBuffer* fb, std::size_t numInstances,
    std::size_t vertices, std::size_t offset) {
    // PHENYL_ASSERT(m_testFramebuffer->renderingRecorder);
    // auto& cmd = *m_testFramebuffer->renderingRecorder;
    auto& cmdList = static_cast<CommandList&>(list);

    if (fb) {
        prepareRender(cmdList.cmd, reinterpret_cast<VulkanFrameBuffer&>(*fb));
    } else {
        prepareRender(cmdList.cmd, *m_windowFrameBuffer);
    }

    if (m_indexBuffer) {
        PHENYL_ASSERT(m_indexBuffer->getBuffer());
        cmdList.cmd.bindIndexBuffer(m_indexBuffer->getBuffer(), m_indexBufferType);
        cmdList.cmd.drawIndexed(numInstances, vertices, offset);
    } else {
        cmdList.cmd.draw(numInstances, vertices, offset);
    }
}

VulkanPipelineFactory::VulkanPipelineFactory (VulkanResources& resources, std::shared_ptr<graphics::Shader> shader,
    VulkanResource<VkPipelineLayout> pipelineLayout, std::vector<VkVertexInputBindingDescription> vertexBindings,
    std::vector<VkVertexInputAttributeDescription> vertexAttribs, VkPrimitiveTopology topology,
    VkCullModeFlags cullMode, const VkPipelineColorBlendAttachmentState& blendAttachment,
    const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo) :
    m_resources{resources},
    m_shader{std::move(shader)},
    m_layout{std::move(pipelineLayout)},
    m_vertexBindings{std::move(vertexBindings)},
    m_attribs{std::move(vertexAttribs)},
    m_colorBlendState{blendAttachment},
    m_depthStencilInfo{depthStencilInfo} {
    m_inputAssemblyInfo = VkPipelineInputAssemblyStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = topology,
      .primitiveRestartEnable = VK_FALSE,
    };

    m_rasterizerInfo = VkPipelineRasterizationStateCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .depthClampEnable = VK_FALSE, // TODO
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = cullMode,
      .frontFace = VK_FRONT_FACE_CLOCKWISE, // TODO

      .depthBiasEnable = VK_FALSE,
      .lineWidth = 1.0f,
    };
}

VkPipeline VulkanPipelineFactory::get (const FrameBufferLayout* layout) {
    PHENYL_ASSERT(layout);

    auto it = m_pipelines.find(layout);
    if (it != m_pipelines.end()) {
        return *it->second;
    }

    VkPipelineVertexInputStateCreateInfo vertexInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
      .vertexBindingDescriptionCount = static_cast<std::uint32_t>(m_vertexBindings.size()),
      .pVertexBindingDescriptions = m_vertexBindings.data(),
      .vertexAttributeDescriptionCount = static_cast<std::uint32_t>(m_attribs.size()),
      .pVertexAttributeDescriptions = m_attribs.data(),
    };

    std::array dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data(),
    };

    VkPipelineViewportStateCreateInfo viewportStateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .scissorCount = 1,
    };

    VkPipelineMultisampleStateCreateInfo multisampleInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .logicOpEnable = VK_FALSE,
      .attachmentCount = 1,
      .pAttachments = &m_colorBlendState,
    };

    PHENYL_ASSERT(m_shader);
    auto& vkShader = reinterpret_cast<VulkanShader&>(m_shader->getUnderlying());
    auto shaderStages = vkShader.getStageInfo();

    auto renderInfo = layout->getInfo();

    VkGraphicsPipelineCreateInfo pipelineInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = &renderInfo,
      .stageCount = static_cast<std::uint32_t>(shaderStages.size()),
      .pStages = shaderStages.data(),

      .pVertexInputState = &vertexInfo,
      .pInputAssemblyState = &m_inputAssemblyInfo,
      .pViewportState = &viewportStateInfo,
      .pRasterizationState = &m_rasterizerInfo,
      .pMultisampleState = &multisampleInfo,
      .pDepthStencilState = &m_depthStencilInfo,
      .pColorBlendState = &colorBlendInfo,
      .pDynamicState = &dynamicStateInfo,
      .layout = *m_layout,
    };

    auto pipeline = m_resources.makePipeline(pipelineInfo);
    if (!pipeline) {
        PHENYL_LOGE(LOGGER, "Failed to construct pipeline for layout {}", reinterpret_cast<const void*>(layout));
        return nullptr;
    }

    auto pipelineRef = *pipeline;
    m_pipelines.emplace(layout, std::move(pipeline));
    return pipelineRef;
}

void VulkanPipeline::prepareRender (VulkanCommandBuffer2& cmd, IVulkanFrameBuffer& frameBuffer) {
    auto descriptorSet = getDescriptorSet(cmd);
    auto sets = std::array{descriptorSet};

    cmd.beginRendering(frameBuffer);
    cmd.setPipeline(m_pipelineFactory->get(frameBuffer.layout()), frameBuffer.viewport(), frameBuffer.scissor());

    m_boundVkBuffers =
        m_boundVertexBuffers | std::views::transform(&IVulkanStorageBuffer::getBuffer) | std::ranges::to<std::vector>();
    cmd.bindVertexBuffers(m_boundVkBuffers, m_vertexBufferOffsets);

    cmd.bindDescriptorSets(m_pipelineFactory->layout(), sets);
}

VkDescriptorSet VulkanPipeline::getDescriptorSet (VulkanCommandBuffer2& cmd) {
    // TODO: only allocate when necessary
    // TODO: incremental updates
    auto set = m_descriptorPools->get().makeDescriptorSet(*m_descriptorSetLayout);

    std::vector<VkWriteDescriptorSet> writes;
    writes.reserve(m_boundUniformBuffers.size());

    std::vector<VkDescriptorBufferInfo> bufferDescriptors;
    bufferDescriptors.reserve(m_boundUniformBuffers.size());
    for (const auto& [binding, bufferInfo] : m_boundUniformBuffers) {
        bufferDescriptors.push_back(bufferInfo.buffer->getBufferInfo(bufferInfo.offset, bufferInfo.size));
        writes.push_back(VkWriteDescriptorSet{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .dstSet = set,
          .dstBinding = binding,
          .dstArrayElement = 0,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
          .pBufferInfo = &bufferDescriptors.back(),
        });
    }

    std::vector<VkDescriptorImageInfo> imageDescriptors;
    imageDescriptors.reserve(m_boundSamplers.size());
    for (auto [binding, sampler] : m_boundSamplers) {
        sampler->prepareSampler(cmd);

        imageDescriptors.push_back(sampler->getDescriptor());
        writes.push_back(VkWriteDescriptorSet{
          .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
          .dstSet = set,
          .dstBinding = binding,
          .dstArrayElement = 0,
          .descriptorCount = 1,
          .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
          .pImageInfo = &imageDescriptors.back(),
        });
    }

    vkUpdateDescriptorSets(m_device, writes.size(), writes.data(), 0, nullptr);

    return set;
}

VulkanPipelineBuilder::VulkanPipelineBuilder (VulkanResources& resources, FrameManager& frameManager,
    VulkanWindowFrameBuffer* windowFb) :
    m_resources{resources},
    m_frameManager{frameManager},
    m_windowFb{windowFb} {
    PHENYL_DASSERT(windowFb);
}

void VulkanPipelineBuilder::withBlendMode (graphics::BlendMode mode) {
    switch (mode) {
    case graphics::BlendMode::ALPHA_BLEND:
        m_blendAttachment = VkPipelineColorBlendAttachmentState{
          .blendEnable = VK_TRUE,
          .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
          .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
          .colorBlendOp = VK_BLEND_OP_ADD,
          .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
          .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
          .alphaBlendOp = VK_BLEND_OP_ADD,
          .colorWriteMask =
              VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        break;
    case graphics::BlendMode::ADDITIVE:
        m_blendAttachment = VkPipelineColorBlendAttachmentState{
          .blendEnable = VK_TRUE,
          .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
          .dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
          .colorBlendOp = VK_BLEND_OP_ADD,
          .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
          .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
          .alphaBlendOp = VK_BLEND_OP_ADD,
          .colorWriteMask =
              VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        break;
    }
}

void VulkanPipelineBuilder::withCullMode (CullMode mode) {
    switch (mode) {
    case graphics::CullMode::FRONT_FACE:
        m_cullMode = VK_CULL_MODE_FRONT_BIT;
        break;
    case graphics::CullMode::BACK_FACE:
        m_cullMode = VK_CULL_MODE_BACK_BIT;
        break;
    case graphics::CullMode::NONE:
        m_cullMode = VK_CULL_MODE_NONE;
        break;
    }
}

void VulkanPipelineBuilder::withDepthTesting (bool doDepthWrite) {
    m_depthTest = true;
    m_depthWrite = doDepthWrite;
}

void VulkanPipelineBuilder::withGeometryType (graphics::GeometryType type) {
    switch (type) {
    case graphics::GeometryType::TRIANGLES:
        m_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    case graphics::GeometryType::LINES:
        m_topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    }
}

void VulkanPipelineBuilder::withShader (const std::shared_ptr<graphics::Shader>& shader) {
    PHENYL_ASSERT(shader);
    this->m_shader = std::move(shader);
}

phenyl::graphics::BufferBinding VulkanPipelineBuilder::withBuffer (meta::TypeIndex type, std::size_t size,
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

    auto binding = static_cast<std::uint32_t>(m_vertexBindings.size());
    m_vertexBindings.push_back(VkVertexInputBindingDescription{
      .binding = binding,
      .stride = static_cast<std::uint32_t>(size),
      .inputRate = vkInputRate,
    });
    m_vertexBindingTypes.emplace_back(type);

    return graphics::BufferBinding{binding};
}

void VulkanPipelineBuilder::withAttrib (graphics::ShaderDataType type, unsigned int location,
    graphics::BufferBinding binding, std::size_t offset) {
    PHENYL_ASSERT(binding < m_vertexBindings.size());
    PHENYL_ASSERT_MSG(location < MAX_ATTRIB_LOCATION, "Attrib location {} larger than defined maximum of {}!", location,
        MAX_ATTRIB_LOCATION);

    switch (type) {
    case graphics::ShaderDataType::FLOAT32:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32_SFLOAT, offset));
        break;
    case graphics::ShaderDataType::INT16:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R16_SINT, offset));
        break;
    case graphics::ShaderDataType::INT32:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32_SINT, offset));
        break;
    case graphics::ShaderDataType::VEC2F:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32_SFLOAT, offset));
        break;
    case graphics::ShaderDataType::VEC3F:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32_SFLOAT, offset));
        break;
    case graphics::ShaderDataType::VEC4F:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32A32_SFLOAT, offset));
        break;
    case graphics::ShaderDataType::MAT2F:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32_SFLOAT, offset));
        m_vertexAttribs.push_back(
            MakeAttrib(binding, location + 1, VK_FORMAT_R32G32_SFLOAT, offset + sizeof(glm::vec2)));
        break;
    case graphics::ShaderDataType::MAT3F:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32_SFLOAT, offset));
        m_vertexAttribs.push_back(
            MakeAttrib(binding, location + 1, VK_FORMAT_R32G32B32_SFLOAT, offset + sizeof(glm::vec3)));
        m_vertexAttribs.push_back(
            MakeAttrib(binding, location + 2, VK_FORMAT_R32G32B32_SFLOAT, offset + sizeof(glm::vec3) * 2));
        break;
    case graphics::ShaderDataType::MAT4F:
        m_vertexAttribs.push_back(MakeAttrib(binding, location, VK_FORMAT_R32G32B32A32_SFLOAT, offset));
        m_vertexAttribs.push_back(
            MakeAttrib(binding, location + 1, VK_FORMAT_R32G32B32A32_SFLOAT, offset + sizeof(glm::vec4)));
        m_vertexAttribs.push_back(
            MakeAttrib(binding, location + 2, VK_FORMAT_R32G32B32A32_SFLOAT, offset + sizeof(glm::vec4) * 2));
        m_vertexAttribs.push_back(
            MakeAttrib(binding, location + 3, VK_FORMAT_R32G32B32A32_SFLOAT, offset + sizeof(glm::vec4) * 3));
        break;
    default:
        PHENYL_LOGE(LOGGER, "Unexpected shader type: {}", static_cast<std::uint32_t>(type));
    }
}

UniformBinding VulkanPipelineBuilder::withUniform (meta::TypeIndex type, unsigned int location) {
    PHENYL_ASSERT_MSG(location < MAX_DESCRIPTOR_BINDING, "Uniform binding {} larger than defined maximum of {}!",
        location, MAX_DESCRIPTOR_BINDING);
    m_descriptorBindings.push_back(VkDescriptorSetLayoutBinding{
      .binding = location,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS, // TODO
    });
    m_uniformTypes[UniformBinding{location}] = type;

    return UniformBinding{location};
}

SamplerBinding VulkanPipelineBuilder::withSampler (unsigned int location) {
    PHENYL_ASSERT_MSG(location < MAX_DESCRIPTOR_BINDING, "Sampler binding {} larger than defined maximum of {}!",
        location, MAX_DESCRIPTOR_BINDING);
    m_descriptorBindings.push_back(VkDescriptorSetLayoutBinding{
      .binding = location,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS, // TODO
    });
    m_registeredSamplers.emplace(location);

    return SamplerBinding{location};
}

std::unique_ptr<phenyl::graphics::IPipeline> VulkanPipelineBuilder::build () {
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
      .depthTestEnable = m_depthTest ? VK_TRUE : VK_FALSE,
      .depthWriteEnable = m_depthTest && m_depthWrite ? VK_TRUE : VK_FALSE,
      .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE,
    };

    DescriptorSizes descriptorSizes{
      .uniforms = static_cast<std::uint32_t>(m_uniformTypes.size()),
      .samplers = static_cast<std::uint32_t>(m_registeredSamplers.size()),
    };
    auto poolFactory = [descriptorSizes, resources = &m_resources] () {
        return VulkanDescriptorPool{*resources, descriptorSizes};
    };
    auto descriptorPools = std::make_unique<FramePool<VulkanDescriptorPool>>(m_frameManager, std::move(poolFactory),
        [] (auto& pool) { pool.reset(); });

    // TODO: multiple sets
    VkDescriptorSetLayoutCreateInfo layoutInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .bindingCount = static_cast<std::uint32_t>(m_descriptorBindings.size()),
      .pBindings = m_descriptorBindings.data(),
    };
    auto descriptorSetLayout = m_resources.makeDescriptorSetLayout(layoutInfo);
    if (!descriptorSetLayout) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline");
        return nullptr;
    }

    VkDescriptorSetLayout dsLayout = *descriptorSetLayout;
    // TODO
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .setLayoutCount = 1,
      .pSetLayouts = &dsLayout,
    };

    auto pipelineLayout = m_resources.makePipelineLayout(pipelineLayoutInfo);
    if (!pipelineLayout) {
        PHENYL_LOGE(LOGGER, "Failed to build pipeline");
        return nullptr;
    }
    PHENYL_DASSERT(pipelineLayout);

    PHENYL_ASSERT(m_shader);
    auto& vkShader = reinterpret_cast<VulkanShader&>(m_shader->getUnderlying());
    auto shaderStages = vkShader.getStageInfo();

    auto pipelineFactory = std::make_unique<VulkanPipelineFactory>(m_resources, std::move(m_shader),
        std::move(pipelineLayout), std::move(m_vertexBindings), std::move(m_vertexAttribs), m_topology, m_cullMode,
        m_blendAttachment, depthStencilInfo);
    return std::make_unique<VulkanPipeline>(m_resources.getDevice(), std::move(pipelineFactory),
        std::move(descriptorSetLayout), std::move(descriptorPools), m_windowFb, std::move(m_vertexBindingTypes),
        std::move(m_uniformTypes), std::move(m_registeredSamplers));
}

static VkVertexInputAttributeDescription MakeAttrib (phenyl::graphics::BufferBinding binding, unsigned int location,
    VkFormat format, std::size_t offset) {
    return VkVertexInputAttributeDescription{.location = location,
      .binding = binding,
      .format = format,
      .offset = static_cast<std::uint32_t>(offset)};
}
