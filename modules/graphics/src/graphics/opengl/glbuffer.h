#pragma once

#include "graphics/graphics_headers.h"
#include "graphics/buffer.h"

namespace phenyl::graphics {
    class GlBuffer : public IBuffer {
    private:
        GLuint bufferId = 0;
        GLenum usageHint;
        std::size_t capacity;
        std::size_t elemSize;

        void ensureCapacity (std::size_t requiredCapacity);
    public:
        GlBuffer (std::size_t capacity, std::size_t elemSize, GLenum usageHint = GL_DYNAMIC_DRAW);

        GlBuffer (const GlBuffer&) = delete;
        GlBuffer (GlBuffer&& other) noexcept;

        GlBuffer& operator= (const GlBuffer&) = delete;
        GlBuffer& operator= (GlBuffer&& other) noexcept;

        ~GlBuffer() override;

        void upload(unsigned char* data, std::size_t size) override;

        void bind () const;

        [[nodiscard]] GLuint id () const {
            return bufferId;
        }

        std::size_t elementSize () const {
            return elemSize;
        }
    };
}