#pragma once

#include "opengl_headers.h"

#include "glsampler.h"
#include "graphics/backend/framebuffer.h"
#include "graphics/viewport.h"

namespace phenyl::opengl {
    class AbstractGlFrameBuffer : public graphics::IFrameBuffer {
    protected:
        GLuint fbId{};
        glm::ivec2 dimensions;

    public:
        AbstractGlFrameBuffer (GLuint fbId, glm::ivec2 dimensions);
        AbstractGlFrameBuffer (glm::ivec2 dimensions);

        AbstractGlFrameBuffer (const AbstractGlFrameBuffer&) = delete;
        AbstractGlFrameBuffer (AbstractGlFrameBuffer&&) noexcept;

        AbstractGlFrameBuffer& operator= (const AbstractGlFrameBuffer&) = delete;
        AbstractGlFrameBuffer& operator= (AbstractGlFrameBuffer&&) noexcept;

        ~AbstractGlFrameBuffer() noexcept override;

        void bind () const;
        void bindViewport () const;
        void clear (glm::vec4 clearColor) override;

        GLuint id () const noexcept {
            return fbId;
        }

        glm::ivec2 getDimensions () const noexcept override;
    };

    class GlFrameBuffer : public AbstractGlFrameBuffer {
    private:
        std::optional<GlSampler> colorSampler;
        std::optional<GlSampler> depthSampler;
        bool valid;
    public:
        GlFrameBuffer (glm::ivec2 dimensions, const graphics::FrameBufferProperties& properties);

        explicit operator bool () const noexcept {
            return valid;
        }

        const graphics::ISampler* getSampler () const noexcept override;
        const graphics::ISampler* getDepthSampler() const noexcept override;
    };

    class GlWindowFrameBuffer : public AbstractGlFrameBuffer, public graphics::IViewportUpdateHandler {
    public:
        GlWindowFrameBuffer (glm::ivec2 dimensions);

        const graphics::ISampler* getSampler() const noexcept override;
        const graphics::ISampler* getDepthSampler() const noexcept override;
        void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
    };
}
