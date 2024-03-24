#include "graphics/detail/loggers.h"
#include "glbuffer.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_BUFFER", detail::GRAPHICS_LOGGER};

GlBuffer::GlBuffer (std::size_t capacity, GLenum usageHint) : capacity{0}, usageHint{usageHint} {
    glGenBuffers(1, &bufferId);

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

    capacity = std::bit_ceil(requiredCapacity);
    bind();
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(capacity), nullptr, usageHint);
}

void GlBuffer::upload (unsigned char* data, std::size_t size) {
    ensureCapacity(size);
    bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
}

void GlBuffer::bind () const {
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
}
