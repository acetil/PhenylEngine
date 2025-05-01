#include "gluniform_buffer.h"
#include "logging/logging.h"

using namespace phenyl::opengl;

GlUniformBuffer::GlUniformBuffer (bool readable) : m_id{}, m_readable{readable} {
    glCreateBuffers(1, &m_id);
}

GlUniformBuffer::GlUniformBuffer (GlUniformBuffer&& other) noexcept : m_id{other.m_id}, m_data{other.m_data}, m_readable{other.m_readable} {
    other.m_id = 0;
    other.m_data = {};
}

GlUniformBuffer& GlUniformBuffer::operator= (GlUniformBuffer&& other) noexcept {
    if (!m_data.empty()) {
        glUnmapBuffer(m_id);
    }

    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }

    m_id = other.m_id;
    m_data = other.m_data;
    m_readable = other.m_readable;

    other.m_id = 0;
    other.m_data = {};

    return *this;
}

GlUniformBuffer::~GlUniformBuffer () {
    /*if (data) {
        glUnmapBuffer(bufferId);
    }*/

    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }
}

std::span<std::byte> GlUniformBuffer::allocate (std::size_t requestSize) {
    PHENYL_DASSERT(requestSize > 0);

    if (m_data.size() >= requestSize) {
        return m_data.subspan(0, requestSize);
    }

    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT;
    if (m_readable) {
        flags |= GL_MAP_READ_BIT;
    }
    glNamedBufferStorage(m_id, static_cast<GLsizeiptr>(requestSize), nullptr, flags);
    void* ptr = glMapNamedBufferRange(m_id, 0, static_cast<GLsizeiptr>(requestSize), flags);

    m_data = std::span{static_cast<std::byte*>(ptr), requestSize};
    return m_data;
}

void GlUniformBuffer::upload () {
    // No-op
}

bool GlUniformBuffer::isReadable () const {
    return m_readable;
}

std::size_t GlUniformBuffer::getMinAlignment () const noexcept {
    GLint64 alignment;
    glGetInteger64v(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    return static_cast<std::size_t>(alignment);
}
