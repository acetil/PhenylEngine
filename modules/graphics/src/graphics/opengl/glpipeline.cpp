#include "glpipeline.h"
#include "glbuffer.h"
#include "gluniform_buffer.h"
#include "graphics/renderers/renderer.h"
#include "gltexture.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_PIPELINE"};

GlPipeline::GlPipeline () : vaoId{0} {
    glCreateVertexArrays(1, &vaoId);
}


GlPipeline::GlPipeline (GlPipeline&& other) noexcept : vaoId{other.vaoId}, bufferTypes{std::move(other.bufferTypes)}, uniformTypes{std::move(other.uniformTypes)} {
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

void GlPipeline::bindBuffer (std::size_t type, BufferBinding binding, IBuffer& buffer) {
    PHENYL_DASSERT_MSG(binding < bufferTypes.size(), "Attempted to bind buffer to binding {} which does not exist!", binding);
    PHENYL_DASSERT_MSG(type == bufferTypes[binding], "Attempted to bind buffer to binding {} with invalid type", binding);
    auto& glBuffer = reinterpret_cast<GlBuffer&>(buffer);

    glVertexArrayVertexBuffer(vaoId, binding, glBuffer.id(), 0, static_cast<GLsizei>(glBuffer.elementSize()));
}

void GlPipeline::bindUniform (std::size_t type, UniformBinding binding, IUniformBuffer& buffer) {
    PHENYL_DASSERT_MSG(uniformTypes.contains(binding), "Attempted to bind uniform to binding {} which does not exist!", binding);
    PHENYL_DASSERT_MSG(type == uniformTypes[binding], "Attempted to bind uniform to binding {} with invalid type", binding);

    auto& glBuffer = reinterpret_cast<GlUniformBuffer&>(buffer);

    glBindBufferBase(GL_UNIFORM_BUFFER, binding, glBuffer.id());
}

void GlPipeline::bindSampler (SamplerBinding binding, const ITexture& texture) {
    const auto& glTexture = reinterpret_cast<const GlTexture&>(texture);
    glActiveTexture(binding);
    glBindTexture(GL_TEXTURE_2D, glTexture.id());
}

void GlPipeline::render (std::size_t vertices) {
    PHENYL_DASSERT(shader);

    shader->bind();
    glBindVertexArray(vaoId);
    glDrawArrays(renderMode, 0, static_cast<GLsizei>(vertices));
}

void GlPipeline::setRenderMode (GLenum renderMode) {
    this->renderMode = renderMode;
}

void GlPipeline::setShader (phenyl::common::Asset<Shader> shader) {
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

GLuint GlPipeline::getCurrDivisor () const {
    if (renderMode == GL_TRIANGLES) {
        return 3;
    } else if (renderMode == GL_LINES) {
        return 2;
    } else {
        return 0;
    }
}

GlPipelineBuilder::GlPipelineBuilder () : pipeline(std::make_unique<GlPipeline>()) {}

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

void GlPipelineBuilder::withShader (common::Asset<Shader> shader) {
    PHENYL_DASSERT(pipeline);
    pipeline->setShader(std::move(shader));
}

BufferBinding GlPipelineBuilder::withBuffer (std::size_t type, std::size_t size, BufferInputRate inputRate) {
    PHENYL_DASSERT(pipeline);
    GLuint divisor = inputRate == BufferInputRate::INSTANCE ? pipeline->getCurrDivisor() : 0;

    return pipeline->addBuffer(type, divisor);
}

void GlPipelineBuilder::withAttrib (ShaderDataType type, unsigned int location, BufferBinding binding, std::size_t offset) {
    PHENYL_DASSERT(pipeline);
    switch (type) {
        case ShaderDataType::FLOAT:
            pipeline->addAttrib(GL_FLOAT, 1, location, binding, offset);
            break;
        case ShaderDataType::INT:
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
            pipeline->addAttrib(GL_FLOAT, 4, location + 1, binding, offset + sizeof(glm::vec3));
            pipeline->addAttrib(GL_FLOAT, 4, location + 2, binding, offset + sizeof(glm::vec3) * 3);
            break;
        default:
            PHENYL_LOGE(LOGGER, "Unable to setup attrib pointer for shader data type {}", getUniformTypeName(type));
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

std::unique_ptr<IPipeline> GlPipelineBuilder::build () {
    PHENYL_DASSERT(pipeline);
    return std::move(pipeline);
}
