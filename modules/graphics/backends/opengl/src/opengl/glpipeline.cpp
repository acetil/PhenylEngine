#include "glpipeline.h"
#include "glbuffer.h"
#include "glframebuffer.h"
#include "glimage_texture.h"
#include "gluniform_buffer.h"

using namespace phenyl::graphics;
using namespace phenyl::opengl;

static phenyl::Logger LOGGER{"GL_PIPELINE"};

GlPipeline::GlPipeline (GlWindowFrameBuffer* fb) : vaoId{0}, windowFrameBuffer{fb} {
    PHENYL_DASSERT(fb);
    glCreateVertexArrays(1, &vaoId);
}


GlPipeline::GlPipeline (GlPipeline&& other) noexcept : vaoId{other.vaoId}, windowFrameBuffer{other.windowFrameBuffer}, bufferTypes{std::move(other.bufferTypes)}, uniformTypes{std::move(other.uniformTypes)} {
    other.vaoId = 0;
}

GlPipeline& GlPipeline::operator= (GlPipeline&& other) noexcept {
    if (vaoId) {
        glDeleteVertexArrays(1, &vaoId);
    }

    vaoId = other.vaoId;
    bufferTypes = std::move(other.bufferTypes);
    uniformTypes = std::move(other.uniformTypes);

    other.vaoId = 0;

    return *this;
}

GlPipeline::~GlPipeline () {
    if (vaoId) {
        glDeleteVertexArrays(1, &vaoId);
    }
}

void GlPipeline::bindBuffer (std::size_t type, BufferBinding binding, const IBuffer& buffer, std::size_t offset) {
    PHENYL_DASSERT_MSG(binding < bufferTypes.size(), "Attempted to bind buffer to binding {} which does not exist!", binding);
    PHENYL_DASSERT_MSG(type == bufferTypes[binding], "Attempted to bind buffer to binding {} with invalid type", binding);
    auto& glBuffer = reinterpret_cast<const GlBuffer&>(buffer);

    glVertexArrayVertexBuffer(vaoId, binding, glBuffer.id(), static_cast<GLintptr>(offset), static_cast<GLsizei>(glBuffer.elementSize()));
}

void GlPipeline::bindUniform (std::size_t type, UniformBinding binding, const IUniformBuffer& buffer) {
    PHENYL_DASSERT_MSG(uniformTypes.contains(binding), "Attempted to bind uniform to binding {} which does not exist!", binding);
    PHENYL_DASSERT_MSG(type == uniformTypes[binding], "Attempted to bind uniform to binding {} with invalid type", binding);

    auto& glBuffer = reinterpret_cast<const GlUniformBuffer&>(buffer);

    glBindBufferBase(GL_UNIFORM_BUFFER, binding, glBuffer.id());
}

void GlPipeline::bindIndexBuffer (ShaderIndexType type, const IBuffer& buffer) {
    switch (type) {
        case ShaderIndexType::UBYTE:
            indexType = PipelineIndex{
                .typeEnum = GL_UNSIGNED_BYTE,
                .typeSize = sizeof(std::uint8_t)
            };
            break;
        case ShaderIndexType::USHORT:
            indexType = PipelineIndex{
                .typeEnum = GL_UNSIGNED_SHORT,
                .typeSize = sizeof(std::uint16_t)
            };
            break;
        case ShaderIndexType::UINT:
            indexType = PipelineIndex{
                .typeEnum = GL_UNSIGNED_INT,
                .typeSize = sizeof(std::uint32_t)
            };
            break;
        default:
            PHENYL_ABORT("Invalid shader index type: {}", static_cast<unsigned int>(type));
    }

    auto& glBuffer = reinterpret_cast<const GlBuffer&>(buffer);
    glVertexArrayElementBuffer(vaoId, glBuffer.id());
}

void GlPipeline::bindSampler (SamplerBinding binding, ISampler& sampler) {
    const auto& glSampler = reinterpret_cast<const GlSampler&>(sampler);
    glActiveTexture(binding);
    glSampler.bind();
}

void GlPipeline::unbindIndexBuffer () {
    glVertexArrayElementBuffer(vaoId, 0);
    indexType = std::nullopt;
}

void GlPipeline::render (IFrameBuffer* frameBuffer, std::size_t vertices, std::size_t offset) {
    PHENYL_DASSERT(shader);

    getShader().bind();
    glBindVertexArray(vaoId);

    //setBlending();

    bindFrameBuffer(frameBuffer);
    setCulling();
    updateDepthMask();

    if (indexType) {
        glDrawElements(renderMode, static_cast<GLsizei>(vertices), indexType->typeEnum, reinterpret_cast<void*>(offset * indexType->typeSize));
    } else {
        glDrawArrays(renderMode, static_cast<GLsizei>(offset), static_cast<GLsizei>(vertices));
    }
}

void GlPipeline::renderInstanced (IFrameBuffer* frameBuffer, std::size_t numInstances, std::size_t vertices, std::size_t offset) {
    PHENYL_DASSERT(shader);

    getShader().bind();
    glBindVertexArray(vaoId);

    bindFrameBuffer(frameBuffer);
    setCulling();
    updateDepthMask();

    if (indexType) {
        glDrawElementsInstanced(renderMode, static_cast<GLsizei>(vertices), indexType->typeEnum, reinterpret_cast<void*>(offset * indexType->typeSize), static_cast<GLsizei>(numInstances));
    } else {
        glDrawArraysInstanced(renderMode, static_cast<GLsizei>(offset), static_cast<GLsizei>(vertices), static_cast<GLsizei>(numInstances));
    }
}

void GlPipeline::setRenderMode (GLenum renderMode) {
    this->renderMode = renderMode;
}

void GlPipeline::setShader (phenyl::core::Asset<Shader> shader) {
    this->shader = std::move(shader);
}

BufferBinding GlPipeline::addBuffer (std::size_t type, GLuint divisor) {
    PHENYL_ASSERT(bufferTypes.size() < GL_MAX_VERTEX_ATTRIB_BINDINGS);

    auto nextBinding = static_cast<BufferBinding>(bufferTypes.size());
    bufferTypes.emplace_back(type);

    glVertexArrayBindingDivisor(vaoId, nextBinding, divisor);

    return nextBinding;
}

void GlPipeline::addAttrib (GLenum type, GLint size, GLuint location, BufferBinding binding, std::size_t offset) {
    PHENYL_ASSERT(location < GL_MAX_VERTEX_ATTRIBS);

    glEnableVertexArrayAttrib(vaoId, location);
    glVertexArrayAttribFormat(vaoId, location, size, type, GL_FALSE, static_cast<GLuint>(offset));
    glVertexArrayAttribBinding(vaoId, location, binding);
}

UniformBinding GlPipeline::addUniform (std::size_t type, unsigned int location) {
    uniformTypes.emplace(location, type);

    return location;
}

SamplerBinding GlPipeline::addSampler (unsigned int location) {
    return GL_TEXTURE0 + location;
}

void GlPipeline::setDepthTest (bool doMask) {
    doDepthTest = true;
    doDepthMask = doMask;
}

void GlPipeline::setBlendMode (BlendMode mode) {
    blendMode = mode;
}

void GlPipeline::setCullMode (CullMode mode) {
    cullMode = mode;
}

GlShader& GlPipeline::getShader () {
    PHENYL_ASSERT(shader);
    return static_cast<GlShader&>(shader->getUnderlying());
}

void GlPipeline::updateDepthMask () {
    if (doDepthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(doDepthMask);
    } else {
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_NONE);
    }
}

void GlPipeline::setBlending (const AbstractGlFrameBuffer& fb) {
    switch (blendMode) {
        case BlendMode::ALPHA_BLEND:
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendMode::ADDITIVE:
            //glBlendFunci(fb.id(), GL_ONE, GL_ONE);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);

            glBlendFunc(GL_ONE, GL_ONE);
            break;
    }
}

void GlPipeline::setCulling () {
    switch (cullMode) {
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
        windowFrameBuffer->bindViewport();
        setBlending(*windowFrameBuffer);
    }
}

GlPipelineBuilder::GlPipelineBuilder (GlWindowFrameBuffer* fb) : pipeline(std::make_unique<GlPipeline>(fb)) {}

void GlPipelineBuilder::withGeometryType (GeometryType type) {
    PHENYL_DASSERT(pipeline);

    switch (type) {
        case GeometryType::TRIANGLES:
            pipeline->setRenderMode(GL_TRIANGLES);
            break;
        case GeometryType::LINES:
            pipeline->setRenderMode(GL_LINES);
            break;
    }
}

void GlPipelineBuilder::withShader (core::Asset<Shader> shader) {
    PHENYL_DASSERT(pipeline);
    pipeline->setShader(std::move(shader));
}

BufferBinding GlPipelineBuilder::withBuffer (std::size_t type, std::size_t size, BufferInputRate inputRate) {
    PHENYL_DASSERT(pipeline);
    GLuint divisor = inputRate == BufferInputRate::INSTANCE ? 1 : 0;

    return pipeline->addBuffer(type, divisor);
}

void GlPipelineBuilder::withAttrib (ShaderDataType type, unsigned int location, BufferBinding binding, std::size_t offset) {
    PHENYL_DASSERT(pipeline);
    switch (type) {
        case ShaderDataType::FLOAT32:
            pipeline->addAttrib(GL_FLOAT, 1, location, binding, offset);
            break;
        case ShaderDataType::INT32:
            pipeline->addAttrib(GL_INT, 1, location, binding, offset);
            break;
        case ShaderDataType::VEC2F:
            pipeline->addAttrib(GL_FLOAT, 2, location, binding, offset);
            break;
        case ShaderDataType::VEC3F:
            pipeline->addAttrib(GL_FLOAT, 3, location, binding, offset);
            break;
        case ShaderDataType::VEC4F:
            pipeline->addAttrib(GL_FLOAT, 4, location, binding, offset);
            break;
        case ShaderDataType::MAT2F:
            pipeline->addAttrib(GL_FLOAT, 2, location, binding, offset);
            pipeline->addAttrib(GL_FLOAT, 2, location + 1, binding, offset + sizeof(glm::vec2));
            break;
        case ShaderDataType::MAT3F:
            pipeline->addAttrib(GL_FLOAT, 3, location, binding, offset);
            pipeline->addAttrib(GL_FLOAT, 3, location + 1, binding, offset + sizeof(glm::vec3));
            pipeline->addAttrib(GL_FLOAT, 3, location + 1, binding, offset + sizeof(glm::vec3) * 2);
            break;
        case ShaderDataType::MAT4F:
            pipeline->addAttrib(GL_FLOAT, 4, location, binding, offset);
            pipeline->addAttrib(GL_FLOAT, 4, location + 1, binding, offset + sizeof(glm::vec4));
            pipeline->addAttrib(GL_FLOAT, 4, location + 2, binding, offset + sizeof(glm::vec4) * 2);
            pipeline->addAttrib(GL_FLOAT, 4, location + 3, binding, offset + sizeof(glm::vec4) * 3);
            break;
        default:
            PHENYL_LOGE(LOGGER, "Unable to setup attrib pointer for shader data type {}", static_cast<unsigned int>(type));
            break;
    }
}

UniformBinding GlPipelineBuilder::withUniform (std::size_t type, unsigned int location) {
    PHENYL_DASSERT(pipeline);
    return pipeline->addUniform(type, location);
}

SamplerBinding GlPipelineBuilder::withSampler (unsigned int location) {
    PHENYL_DASSERT(pipeline);
    return pipeline->addSampler(location);
}

void GlPipelineBuilder::withCullMode (CullMode mode) {
    PHENYL_DASSERT(pipeline);
    pipeline->setCullMode(mode);
}

void GlPipelineBuilder::withBlendMode (BlendMode mode) {
    PHENYL_DASSERT(pipeline);
    pipeline->setBlendMode(mode);
}

void GlPipelineBuilder::withDepthTesting (bool doDepthWrite) {
    PHENYL_DASSERT(pipeline);
    pipeline->setDepthTest(doDepthWrite);
}

std::unique_ptr<IPipeline> GlPipelineBuilder::build () {
    PHENYL_DASSERT(pipeline);
    return std::move(pipeline);
}
