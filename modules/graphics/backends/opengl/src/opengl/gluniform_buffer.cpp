#include "gluniform_buffer.h"
#include "logging/logging.h"

using namespace phenyl::opengl;

GlUniformBuffer::GlUniformBuffer (bool readable) : bufferId{}, data{nullptr}, size{0}, readable{readable} {
    glCreateBuffers(1, &bufferId);
}

GlUniformBuffer::GlUniformBuffer (GlUniformBuffer&& other) noexcept : bufferId{other.bufferId}, data{other.data}, size{other.size}, readable{other.readable} {
    other.bufferId = 0;
    other.data = nullptr;
    other.size = 0;
}

GlUniformBuffer& GlUniformBuffer::operator= (GlUniformBuffer&& other) noexcept {
    if (data) {
        glUnmapBuffer(bufferId);
    }

    if (bufferId) {
        glDeleteBuffers(1, &bufferId);
    }

    bufferId = other.bufferId;
    data = other.data;
    size = other.size;
    readable = other.readable;

    other.bufferId = 0;
    other.data = nullptr;
    other.size = 0;

    return *this;
}

GlUniformBuffer::~GlUniformBuffer () {
    /*if (data) {
        glUnmapBuffer(bufferId);
    }*/

    if (bufferId) {
        glDeleteBuffers(1, &bufferId);
    }
}

unsigned char* GlUniformBuffer::allocate (std::size_t requestSize) {
    PHENYL_DASSERT(requestSize > 0);

    if (size >= requestSize) {
        return static_cast<unsigned char*>(data);
    }
    size = requestSize;

    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
    if (readable) {
        flags |= GL_MAP_READ_BIT;
    }
    glNamedBufferStorage(bufferId, static_cast<GLsizeiptr>(size), nullptr, flags);
    data = glMapNamedBufferRange(bufferId, 0, static_cast<GLsizeiptr>(size), flags);

    return static_cast<unsigned char*>(data);
}

void GlUniformBuffer::upload () {
    // No-op
}

bool GlUniformBuffer::isReadable () const {
    return readable;
}

std::size_t GlUniformBuffer::getMinAlignment () const noexcept {
    GLint64 alignment;
    glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    return static_cast<std::size_t>(alignment);
}
