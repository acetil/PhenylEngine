#pragma once

#include "graphics/backend/uniform_buffer.h"
#include "opengl_headers.h"

namespace phenyl::opengl {
    class GlUniformBuffer : public graphics::IUniformBuffer {
    private:
        GLuint bufferId;
        void* data;
        std::size_t size;
        bool readable;
    public:
        explicit GlUniformBuffer (bool readable);

        GlUniformBuffer (const GlUniformBuffer&) = delete;
        GlUniformBuffer (GlUniformBuffer&& other) noexcept;

        GlUniformBuffer& operator= (const GlUniformBuffer&) = delete;
        GlUniformBuffer& operator= (GlUniformBuffer&& other) noexcept;

        ~GlUniformBuffer() override;

        unsigned char* allocate (std::size_t requestSize) override;
        void upload () override;
        [[nodiscard]] bool isReadable () const override;
        std::size_t getMinAlignment () const noexcept override;

        [[nodiscard]] GLuint id () const {
            return bufferId;
        }
    };
}