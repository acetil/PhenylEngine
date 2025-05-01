#include "glbuffer.h"

using namespace phenyl::opengl;

static phenyl::Logger LOGGER{"GL_BUFFER", detail::OPENGL_LOGGER};

GlBuffer::GlBuffer (std::size_t capacity, std::size_t elemSize, GLenum usageHint) : m_capacity{0}, m_stride{elemSize}, m_usageHint{usageHint} {
    glCreateBuffers(1, &m_id);
    PHENYL_TRACE(LOGGER, "Initialised buffer id={}", m_id);

    ensureCapacity(capacity);
}

GlBuffer::GlBuffer (GlBuffer&& other) noexcept : m_id{other.m_id}, m_capacity{other.m_capacity}, m_usageHint{other.m_usageHint} {
    other.m_id = 0;
    other.m_capacity = 0;
}

GlBuffer::~GlBuffer () {
    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }
}

GlBuffer& GlBuffer::operator= (GlBuffer&& other) noexcept {
    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }

    m_id = other.m_id;
    m_capacity = other.m_capacity;
    m_usageHint = other.m_usageHint;

    other.m_id = 0;
    other.m_capacity = 0;

    return *this;
}

void GlBuffer::ensureCapacity (std::size_t requiredCapacity) {
    if (requiredCapacity < m_capacity) {
        return;
    }

    PHENYL_TRACE(LOGGER, "Resize requested for buffer id={} from {} to {}", m_id, m_capacity, requiredCapacity);
    m_capacity = std::bit_ceil(requiredCapacity);
    glNamedBufferData(m_id, static_cast<GLsizeiptr>(m_capacity), nullptr, m_usageHint);
    PHENYL_TRACE(LOGGER, "Resized buffer buffer id={} to {}", m_id, m_capacity);
}

void GlBuffer::upload (std::span<const std::byte> data) {
    ensureCapacity(data.size());

    PHENYL_TRACE(LOGGER, "Uploading {} bytes to buffer id={}", data.size(), m_id);
    glNamedBufferSubData(m_id, 0, static_cast<GLsizeiptr>(data.size()), data.data());
}

void GlBuffer::bind () const {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}
