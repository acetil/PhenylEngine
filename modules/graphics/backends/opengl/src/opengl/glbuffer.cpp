#include "glbuffer.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_BUFFER", detail::OPENGL_LOGGER};

GlBuffer::GlBuffer (std::size_t capacity, std::size_t elemSize, GLenum usageHint) : capacity{0}, elemSize{elemSize}, usageHint{usageHint} {
    glCreateBuffers(1, &bufferId);
    PHENYL_TRACE(LOGGER, "Initialised buffer id={}", bufferId);

    ensureCapacity(capacity);
}

GlBuffer::GlBuffer (GlBuffer&& other) noexcept : bufferId{other.bufferId}, capacity{other.capacity}, usageHint{other.usageHint} {
    other.bufferId = 0;
    other.capacity = 0;
}

GlBuffer::~GlBuffer () {
    if (bufferId) {
        glDeleteBuffers(1, &bufferId);
    }
}

GlBuffer& GlBuffer::operator= (GlBuffer&& other) noexcept {
    if (bufferId) {
        glDeleteBuffers(1, &bufferId);
    }

    bufferId = other.bufferId;
    capacity = other.capacity;
    usageHint = other.usageHint;

    other.bufferId = 0;
    other.capacity = 0;

    return *this;
}

void GlBuffer::ensureCapacity (std::size_t requiredCapacity) {
    if (requiredCapacity < capacity) {
        return;
    }

    PHENYL_TRACE(LOGGER, "Resize requested for buffer id={} from {} to {}", bufferId, capacity, requiredCapacity);
    capacity = std::bit_ceil(requiredCapacity);
    glNamedBufferData(bufferId, static_cast<GLsizeiptr>(capacity), nullptr, usageHint);
    PHENYL_TRACE(LOGGER, "Resized buffer buffer id={} to {}", bufferId, capacity);
}

void GlBuffer::upload (unsigned char* data, std::size_t size) {
    ensureCapacity(size);

    PHENYL_TRACE(LOGGER, "Uploading {} bytes to buffer id={}", size, bufferId);
    glNamedBufferSubData(bufferId, 0, static_cast<GLsizeiptr>(size), data);
}

void GlBuffer::bind () const {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
}
