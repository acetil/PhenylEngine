#include "glpipeline.h"

#include "glbuffer.h"
#include "glframebuffer.h"
#include "glimage_texture.h"
#include "gluniform_buffer.h"

using namespace phenyl::graphics;
using namespace phenyl::opengl;

static phenyl::Logger LOGGER{"GL_PIPELINE"};

GlPipeline::GlPipeline (GlWindowFrameBuffer* fb) : m_vao{0}, m_windowFrameBuffer{fb} {
    PHENYL_DASSERT(fb);
    glCreateVertexArrays(1, &m_vao);
}

GlPipeline::GlPipeline (GlPipeline&& other) noexcept :
    m_vao{other.m_vao},
    m_windowFrameBuffer{other.m_windowFrameBuffer},
    m_bufferTypes{std::move(other.m_bufferTypes)},
    m_uniformTypes{std::move(other.m_uniformTypes)} {
    other.m_vao = 0;
}

GlPipeline& GlPipeline::operator= (GlPipeline&& other) noexcept {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
    }

    m_vao = other.m_vao;
    m_bufferTypes = std::move(other.m_bufferTypes);
    m_uniformTypes = std::move(other.m_uniformTypes);

    other.m_vao = 0;

    return *this;
}

GlPipeline::~GlPipeline () {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
    }
}

void GlPipeline::bindBuffer (meta::TypeIndex type, BufferBinding binding, const IBuffer& buffer, std::size_t offset) {
    PHENYL_DASSERT_MSG(binding < m_bufferTypes.size(), "Attempted to bind buffer to binding {} which does not exist!",
        binding);
    PHENYL_DASSERT_MSG(type == m_bufferTypes[binding], "Attempted to bind buffer to binding {} with invalid type",
        binding);
    auto& glBuffer = reinterpret_cast<const GlBuffer&>(buffer);

    glVertexArrayVertexBuffer(m_vao, binding, glBuffer.id(), static_cast<GLintptr>(offset),
        static_cast<GLsizei>(glBuffer.stride()));
}

void GlPipeline::bindUniform (meta::TypeIndex type, UniformBinding binding, const IUniformBuffer& buffer,
    std::size_t offset, std::size_t size) {
    PHENYL_DASSERT_MSG(m_uniformTypes.contains(binding),
        "Attempted to bind uniform to binding {} which does not exist!", binding);
    PHENYL_DASSERT_MSG(type == m_uniformTypes[binding], "Attempted to bind uniform to binding {} with invalid type",
        binding);

    auto& glBuffer = reinterpret_cast<const GlUniformBuffer&>(buffer);

    glBindBufferRange(GL_UNIFORM_BUFFER, binding, glBuffer.id(), static_cast<GLintptr>(offset),
        static_cast<GLsizeiptr>(size));
}

void GlPipeline::bindIndexBuffer (ShaderIndexType type, const IBuffer& buffer) {
    switch (type) {
    case ShaderIndexType::UBYTE:
        m_indexType = PipelineIndex{.typeEnum = GL_UNSIGNED_BYTE, .typeSize = sizeof(std::uint8_t)};
        break;
    case ShaderIndexType::USHORT:
        m_indexType = PipelineIndex{.typeEnum = GL_UNSIGNED_SHORT, .typeSize = sizeof(std::uint16_t)};
        break;
    case ShaderIndexType::UINT:
        m_indexType = PipelineIndex{.typeEnum = GL_UNSIGNED_INT, .typeSize = sizeof(std::uint32_t)};
        break;
    default:
        PHENYL_ABORT("Invalid shader index type: {}", static_cast<unsigned int>(type));
    }

    auto& glBuffer = reinterpret_cast<const GlBuffer&>(buffer);
    glVertexArrayElementBuffer(m_vao, glBuffer.id());
}

void GlPipeline::bindSampler (SamplerBinding binding, ISampler& sampler) {
    const auto& glSampler = reinterpret_cast<const GlSampler&>(sampler);
    glActiveTexture(binding);
    glSampler.bind();
}

void GlPipeline::unbindIndexBuffer () {
    glVertexArrayElementBuffer(m_vao, 0);
    m_indexType = std::nullopt;
}

void GlPipeline::render (ICommandList& list, IFrameBuffer* frameBuffer, std::size_t vertices, std::size_t offset) {
    PHENYL_DASSERT(m_shader);

    getShader().bind();
    glBindVertexArray(m_vao);

    // setBlending();

    bindFrameBuffer(frameBuffer);
    setCulling();
    updateDepthMask();

    if (m_indexType) {
        glDrawElements(m_renderMode, static_cast<GLsizei>(vertices), m_indexType->typeEnum,
            reinterpret_cast<void*>(offset * m_indexType->typeSize));
    } else {
        glDrawArrays(m_renderMode, static_cast<GLsizei>(offset), static_cast<GLsizei>(vertices));
    }
}

void GlPipeline::renderInstanced (ICommandList& list, IFrameBuffer* frameBuffer, std::size_t numInstances,
    std::size_t vertices, std::size_t offset) {
    PHENYL_DASSERT(m_shader);

    getShader().bind();
    glBindVertexArray(m_vao);

    bindFrameBuffer(frameBuffer);
    setCulling();
    updateDepthMask();

    if (m_indexType) {
        glDrawElementsInstanced(m_renderMode, static_cast<GLsizei>(vertices), m_indexType->typeEnum,
            reinterpret_cast<void*>(offset * m_indexType->typeSize), static_cast<GLsizei>(numInstances));
    } else {
        glDrawArraysInstanced(m_renderMode, static_cast<GLsizei>(offset), static_cast<GLsizei>(vertices),
            static_cast<GLsizei>(numInstances));
    }
}

void GlPipeline::setRenderMode (GLenum renderMode) {
    this->m_renderMode = renderMode;
}

void GlPipeline::setShader (std::shared_ptr<Shader> shader) {
    this->m_shader = std::move(shader);
}

BufferBinding GlPipeline::addBuffer (meta::TypeIndex type, GLuint divisor) {
    PHENYL_ASSERT(m_bufferTypes.size() < GL_MAX_VERTEX_ATTRIB_BINDINGS);

    auto nextBinding = static_cast<BufferBinding>(m_bufferTypes.size());
    m_bufferTypes.emplace_back(type);

    glVertexArrayBindingDivisor(m_vao, nextBinding, divisor);

    return nextBinding;
}

void GlPipeline::addAttrib (GLenum type, GLint size, GLuint location, BufferBinding binding, std::size_t offset) {
    PHENYL_ASSERT(location < GL_MAX_VERTEX_ATTRIBS);

    glEnableVertexArrayAttrib(m_vao, location);
    glVertexArrayAttribFormat(m_vao, location, size, type, GL_FALSE, static_cast<GLuint>(offset));
    glVertexArrayAttribBinding(m_vao, location, binding);
}

UniformBinding GlPipeline::addUniform (meta::TypeIndex type, unsigned int location) {
    m_uniformTypes.emplace(location, type);

    return location;
}

SamplerBinding GlPipeline::addSampler (unsigned int location) {
    return GL_TEXTURE0 + location;
}

void GlPipeline::setDepthTest (bool doMask) {
    m_doDepthTest = true;
    m_doDepthMask = doMask;
}

void GlPipeline::setBlendMode (BlendMode mode) {
    m_blendMode = mode;
}

void GlPipeline::setCullMode (CullMode mode) {
    m_cullMode = mode;
}

GlShader& GlPipeline::getShader () {
    PHENYL_ASSERT(m_shader);
    return static_cast<GlShader&>(m_shader->getUnderlying());
}

void GlPipeline::updateDepthMask () {
    if (m_doDepthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(m_doDepthMask);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void GlPipeline::setBlending (const AbstractGlFrameBuffer& fb) {
    switch (m_blendMode) {
    case BlendMode::ALPHA_BLEND:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendMode::ADDITIVE:
        // glBlendFunci(fb.id(), GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);

        glBlendFunc(GL_ONE, GL_ONE);
        break;
    }
}

void GlPipeline::setCulling () {
    switch (m_cullMode) {
    case CullMode::FRONT_FACE:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        break;
    case CullMode::BACK_FACE:
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    case CullMode::NONE:
        glDisable(GL_CULL_FACE);
        break;
    }
}

void GlPipeline::bindFrameBuffer (IFrameBuffer* frameBuffer) {
    if (frameBuffer) {
        auto* fb = reinterpret_cast<AbstractGlFrameBuffer*>(frameBuffer);
        fb->bindViewport();
        setBlending(*fb);
    } else {
        m_windowFrameBuffer->bindViewport();
        setBlending(*m_windowFrameBuffer);
    }
}

GlPipelineBuilder::GlPipelineBuilder (GlWindowFrameBuffer* fb) : m_pipeline(std::make_unique<GlPipeline>(fb)) {}

void GlPipelineBuilder::withGeometryType (GeometryType type) {
    PHENYL_DASSERT(m_pipeline);

    switch (type) {
    case GeometryType::TRIANGLES:
        m_pipeline->setRenderMode(GL_TRIANGLES);
        break;
    case GeometryType::LINES:
        m_pipeline->setRenderMode(GL_LINES);
        break;
    }
}

void GlPipelineBuilder::withShader (const std::shared_ptr<Shader>& shader) {
    PHENYL_DASSERT(m_pipeline);
    m_pipeline->setShader(shader);
}

BufferBinding GlPipelineBuilder::withBuffer (meta::TypeIndex type, std::size_t size, BufferInputRate inputRate) {
    PHENYL_DASSERT(m_pipeline);
    GLuint divisor = inputRate == BufferInputRate::INSTANCE ? 1 : 0;

    return m_pipeline->addBuffer(type, divisor);
}

void GlPipelineBuilder::withAttrib (ShaderDataType type, unsigned int location, BufferBinding binding,
    std::size_t offset) {
    PHENYL_DASSERT(m_pipeline);
    switch (type) {
    case ShaderDataType::FLOAT32:
        m_pipeline->addAttrib(GL_FLOAT, 1, location, binding, offset);
        break;
    case ShaderDataType::INT32:
        m_pipeline->addAttrib(GL_INT, 1, location, binding, offset);
        break;
    case ShaderDataType::VEC2F:
        m_pipeline->addAttrib(GL_FLOAT, 2, location, binding, offset);
        break;
    case ShaderDataType::VEC3F:
        m_pipeline->addAttrib(GL_FLOAT, 3, location, binding, offset);
        break;
    case ShaderDataType::VEC4F:
        m_pipeline->addAttrib(GL_FLOAT, 4, location, binding, offset);
        break;
    case ShaderDataType::MAT2F:
        m_pipeline->addAttrib(GL_FLOAT, 2, location, binding, offset);
        m_pipeline->addAttrib(GL_FLOAT, 2, location + 1, binding, offset + sizeof(glm::vec2));
        break;
    case ShaderDataType::MAT3F:
        m_pipeline->addAttrib(GL_FLOAT, 3, location, binding, offset);
        m_pipeline->addAttrib(GL_FLOAT, 3, location + 1, binding, offset + sizeof(glm::vec3));
        m_pipeline->addAttrib(GL_FLOAT, 3, location + 1, binding, offset + sizeof(glm::vec3) * 2);
        break;
    case ShaderDataType::MAT4F:
        m_pipeline->addAttrib(GL_FLOAT, 4, location, binding, offset);
        m_pipeline->addAttrib(GL_FLOAT, 4, location + 1, binding, offset + sizeof(glm::vec4));
        m_pipeline->addAttrib(GL_FLOAT, 4, location + 2, binding, offset + sizeof(glm::vec4) * 2);
        m_pipeline->addAttrib(GL_FLOAT, 4, location + 3, binding, offset + sizeof(glm::vec4) * 3);
        break;
    default:
        PHENYL_LOGE(LOGGER, "Unable to setup attrib pointer for shader data type {}", static_cast<unsigned int>(type));
        break;
    }
}

UniformBinding GlPipelineBuilder::withUniform (meta::TypeIndex type, unsigned int location) {
    PHENYL_DASSERT(m_pipeline);
    return m_pipeline->addUniform(type, location);
}

SamplerBinding GlPipelineBuilder::withSampler (unsigned int location) {
    PHENYL_DASSERT(m_pipeline);
    return m_pipeline->addSampler(location);
}

void GlPipelineBuilder::withCullMode (CullMode mode) {
    PHENYL_DASSERT(m_pipeline);
    m_pipeline->setCullMode(mode);
}

void GlPipelineBuilder::withBlendMode (BlendMode mode) {
    PHENYL_DASSERT(m_pipeline);
    m_pipeline->setBlendMode(mode);
}

void GlPipelineBuilder::withDepthTesting (bool doDepthWrite) {
    PHENYL_DASSERT(m_pipeline);
    m_pipeline->setDepthTest(doDepthWrite);
}

std::unique_ptr<IPipeline> GlPipelineBuilder::build () {
    PHENYL_DASSERT(m_pipeline);
    return std::move(m_pipeline);
}
