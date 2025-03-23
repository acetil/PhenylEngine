#pragma once

#include "graphics/graphics_headers.h"

#include "glsampler.h"
#include "graphics/framebuffer.h"
#include "graphics/viewport.h"

namespace phenyl::graphics {
    class AbstractGlFrameBuffer : public IFrameBuffer {
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
        GlFrameBuffer (glm::ivec2 dimensions, const FrameBufferProperties& properties);

        explicit operator bool () const noexcept {
            return valid;
        }

        const ISampler* getSampler () const noexcept override;
        const ISampler* getDepthSampler() const noexcept override;
    };

    class GlWindowFrameBuffer : public AbstractGlFrameBuffer, public IViewportUpdateHandler {
    public:
        GlWindowFrameBuffer (glm::ivec2 dimensions);

        const ISampler* getSampler() const noexcept override;
        const ISampler* getDepthSampler() const noexcept override;
        void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
    };
}
