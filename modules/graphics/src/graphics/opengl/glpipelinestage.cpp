#include "glpipelinestage.h"
#include "glbuffer.h"

#include "logging/logging.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_PIPELINE_STAGE", detail::GRAPHICS_LOGGER};

static void enableVertexAttribPointer (GLuint vaoId, const GlBuffer& buffer, int location, GLenum type, GLsizei size, GLsizei stride=0, std::size_t offset=0);
static void setupVertexAttribPointer (GLuint vaoId, const GlBuffer& buffer, int location, ShaderDataType attribType);

static GLenum getRenderMode (PipelineType type);

GLPipelineStage::GLPipelineStage (PipelineStageSpec& spec) : vertexAttribs{std::move(spec.vertexAttribs)} {
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    //std::size_t i = 0;

    renderMode = getRenderMode(spec.type);
}

void GLPipelineStage::bindBuffer (int location, ShaderDataType attribType, const IBuffer& buffer) {
    if (vertexAttribs[location] != attribType) {
        PHENYL_LOGE(LOGGER, "Attempted to bind buffer of type {} to attribute of type {}!", getUniformTypeName(attribType),
                    getUniformTypeName(vertexAttribs[location]));
        return;
    }

    const auto& glBuffer = reinterpret_cast<const GlBuffer&>(buffer);

    glBindVertexArray(vaoId);
    setupVertexAttribPointer(vaoId, glBuffer, location, vertexAttribs[location]);
}

void GLPipelineStage::render (std::size_t numVertices) {
    //logging::log(LEVEL_DEBUG, "Drawing {} vertices!", numVertices);
    glBindVertexArray(vaoId);
    glDrawArrays(renderMode, 0, static_cast<GLsizei>(numVertices));
}

GLPipelineStage::~GLPipelineStage () {
    glDeleteVertexArrays(1, &vaoId);
}

static void setupVertexAttribPointer (GLuint vaoId, const GlBuffer& buffer, int location, ShaderDataType attribType) {
    //glBindVertexArray(vaoId);
    switch (attribType) {
        case ShaderDataType::FLOAT:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 1);
            break;
        case ShaderDataType::INT:
            enableVertexAttribPointer(vaoId, buffer, location, GL_INT, 1);
            break;
        case ShaderDataType::VEC2F:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 2);
            break;
        case ShaderDataType::VEC3F:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 3);
            break;
        case ShaderDataType::VEC4F:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 4);
            break;
        case ShaderDataType::MAT2F:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 2, sizeof(float) * 4);
            enableVertexAttribPointer(vaoId, buffer, location + 1, GL_FLOAT, 2, sizeof(float) * 4, sizeof(float) * 2);
            break;
        case ShaderDataType::MAT3F:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 3, sizeof(float) * 9);
            enableVertexAttribPointer(vaoId, buffer, location + 1, GL_FLOAT, 3, sizeof(float) * 9, sizeof(float) * 3);
            enableVertexAttribPointer(vaoId, buffer, location + 2, GL_FLOAT, 3, sizeof(float) * 9, sizeof(float) * 6);
            break;
        case ShaderDataType::MAT4F:
            enableVertexAttribPointer(vaoId, buffer, location, GL_FLOAT, 4, sizeof(float) * 16);
            enableVertexAttribPointer(vaoId, buffer, location + 1, GL_FLOAT, 4, sizeof(float) * 16, sizeof(float) * 4);
            enableVertexAttribPointer(vaoId, buffer, location + 2, GL_FLOAT, 4, sizeof(float) * 16, sizeof(float) * 8);
            enableVertexAttribPointer(vaoId, buffer, location + 3, GL_FLOAT, 4, sizeof(float) * 16, sizeof(float) * 12);
            break;
        default:
        PHENYL_LOGE(LOGGER, "Unable to setup attrib pointer for shader data type {}", getUniformTypeName(attribType));
    }
}

static void enableVertexAttribPointer (GLuint vaoId, const GlBuffer& buffer, int location, GLenum type, GLsizei size, GLsizei stride, std::size_t offset) {
    glBindVertexArray(vaoId);
    buffer.bind();
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, type, GL_FALSE, stride, (void*)offset);
}

static GLenum getRenderMode (PipelineType type) {
    switch (type) {
        case PipelineType::TRIANGLES:
            return GL_TRIANGLES;
        case PipelineType::LINES:
            return GL_LINES;
        default:
            return 0;
    }
}