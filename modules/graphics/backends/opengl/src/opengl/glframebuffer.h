#pragma once

#include "glsampler.h"
#include "graphics/backend/framebuffer.h"
#include "graphics/viewport.h"
#include "opengl_headers.h"

namespace phenyl::opengl {
class AbstractGlFrameBuffer : public graphics::IFrameBuffer {
public:
    AbstractGlFrameBuffer (GLuint fbId, glm::ivec2 dimensions);
    AbstractGlFrameBuffer (glm::ivec2 dimensions);

    AbstractGlFrameBuffer (const AbstractGlFrameBuffer&) = delete;
    AbstractGlFrameBuffer (AbstractGlFrameBuffer&&) noexcept;

    AbstractGlFrameBuffer& operator= (const AbstractGlFrameBuffer&) = delete;
    AbstractGlFrameBuffer& operator= (AbstractGlFrameBuffer&&) noexcept;

    ~AbstractGlFrameBuffer () noexcept override;

    void bind () const;
    void bindViewport () const;
    void clear (glm::vec4 clearColor) override;

    GLuint id () const noexcept {
        return m_id;
    }

    glm::ivec2 getDimensions () const noexcept override;

protected:
    GLuint m_id{};
    glm::ivec2 m_dimensions;
};

class GlFrameBuffer : public AbstractGlFrameBuffer {
public:
    GlFrameBuffer (glm::ivec2 dimensions, const graphics::FrameBufferProperties& properties);

    explicit operator bool () const noexcept {
        return m_valid;
    }

    graphics::ISampler* getSampler () noexcept override;
    graphics::ISampler* getDepthSampler () noexcept override;

private:
    std::optional<GlSampler> m_colorSampler;
    std::optional<GlSampler> m_depthSampler;
    bool m_valid;
};

class GlWindowFrameBuffer : public AbstractGlFrameBuffer, public graphics::IViewportUpdateHandler {
public:
    GlWindowFrameBuffer (glm::ivec2 dimensions);

    graphics::ISampler* getSampler () noexcept override;
    graphics::ISampler* getDepthSampler () noexcept override;
    void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
};
} // namespace phenyl::opengl
