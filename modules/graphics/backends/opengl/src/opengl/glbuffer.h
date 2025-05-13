#pragma once

#include "graphics/backend/buffer.h"
#include "opengl_headers.h"

namespace phenyl::opengl {
class GlBuffer : public graphics::IBuffer {
public:
    GlBuffer (std::size_t capacity, std::size_t elemSize, GLenum usageHint = GL_DYNAMIC_DRAW);

    GlBuffer (const GlBuffer&) = delete;
    GlBuffer (GlBuffer&& other) noexcept;

    GlBuffer& operator= (const GlBuffer&) = delete;
    GlBuffer& operator= (GlBuffer&& other) noexcept;

    ~GlBuffer () override;

    void upload (std::span<const std::byte> data) override;

    void bind () const;

    [[nodiscard]] GLuint id () const {
        return m_id;
    }

    std::size_t stride () const {
        return m_stride;
    }

private:
    GLuint m_id = 0;
    GLenum m_usageHint;
    std::size_t m_capacity;
    std::size_t m_stride;

    void ensureCapacity (std::size_t requiredCapacity);
};
} // namespace phenyl::opengl
