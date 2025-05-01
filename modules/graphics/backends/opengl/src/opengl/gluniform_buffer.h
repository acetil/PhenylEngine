#pragma once

#include "graphics/backend/uniform_buffer.h"
#include "opengl_headers.h"

namespace phenyl::opengl {
    class GlUniformBuffer : public graphics::IUniformBuffer {
    public:
        explicit GlUniformBuffer (bool readable);

        GlUniformBuffer (const GlUniformBuffer&) = delete;
        GlUniformBuffer (GlUniformBuffer&& other) noexcept;

        GlUniformBuffer& operator= (const GlUniformBuffer&) = delete;
        GlUniformBuffer& operator= (GlUniformBuffer&& other) noexcept;

        ~GlUniformBuffer() override;

        std::span<std::byte> allocate (std::size_t requestSize) override;
        void upload () override;
        [[nodiscard]] bool isReadable () const override;
        std::size_t getMinAlignment () const noexcept override;

        [[nodiscard]] GLuint id () const {
            return m_id;
        }

    private:
        GLuint m_id;
        std::span<std::byte> m_data{};
        bool m_readable;
    };
}