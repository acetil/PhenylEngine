#pragma once

#include "graphics/graphics_headers.h"
#include "graphics/renderers/buffer.h"

namespace phenyl::graphics {
    class GlBuffer : public IBuffer {
    private:
        GLuint bufferId = 0;
        GLenum usageHint;
        std::size_t capacity;

        void ensureCapacity (std::size_t requiredCapacity);
    public:
        GlBuffer (std::size_t capacity, GLenum usageHint = GL_DYNAMIC_DRAW);

        GlBuffer (const GlBuffer&) = delete;
        GlBuffer (GlBuffer&& other) noexcept;

        GlBuffer& operator= (const GlBuffer&) = delete;
        GlBuffer& operator= (GlBuffer&& other) noexcept;

        ~GlBuffer() override;

        void upload(unsigned char* data, std::size_t size) override;

        void bind () const;
    };
}