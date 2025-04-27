#include "gluniform_buffer.h"
#include "logging/logging.h"

using namespace phenyl::opengl;

GlUniformBuffer::GlUniformBuffer (bool readable) : bufferId{}, readable{readable} {
    glCreateBuffers(1, &bufferId);
}

GlUniformBuffer::GlUniformBuffer (GlUniformBuffer&& other) noexcept : bufferId{other.bufferId}, data{other.data}, readable{other.readable} {
    other.bufferId = 0;
    other.data = {};
}

GlUniformBuffer& GlUniformBuffer::operator= (GlUniformBuffer&& other) noexcept {
    if (!data.empty()) {
        glUnmapBuffer(bufferId);
    }

    if (bufferId) {
        glDeleteBuffers(1, &bufferId);
    }

    bufferId = other.bufferId;
    data = other.data;
    readable = other.readable;

    other.bufferId = 0;
    other.data = {};

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

std::span<std::byte> GlUniformBuffer::allocate (std::size_t requestSize) {
    PHENYL_DASSERT(requestSize > 0);

    if (data.size() >= requestSize) {
        return data.subspan(0, requestSize);
    }

    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
    if (readable) {
        flags |= GL_MAP_READ_BIT;
    }
    glNamedBufferStorage(bufferId, static_cast<GLsizeiptr>(requestSize), nullptr, flags);
    void* ptr = glMapNamedBufferRange(bufferId, 0, static_cast<GLsizeiptr>(requestSize), flags);

    data = std::span{static_cast<std::byte*>(ptr), requestSize};
    return data;
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
