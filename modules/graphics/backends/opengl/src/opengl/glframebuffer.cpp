#include "glframebuffer.h"

using namespace phenyl::opengl;

static phenyl::Logger LOGGER{"GL_FRAME_BUFFER", detail::OPENGL_LOGGER};

static std::string_view GetFrameBufferStatusString (GLenum status) {
    if (status == GL_FRAMEBUFFER_UNDEFINED) {
        return "FRAMEBUFFER_UNDEFINED";
    } else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
        return "INCOMPLETE_ATTACHMENT";
    } else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
        return "INCOMPLETE_MISSING_ATTACHMENT";
    } else if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
        return "INCOMPLETE_DRAW_BUFFER";
    } else if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
        return "INCOMPLETE_READ_BUFFER";
    } else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
        return "UNSUPPORTED";
    } else if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) {
        return "INCOMPLETE_MULTISAMPLE";
    } else if (status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) {
        return "INCOMPLETE_LAYER_TARGETS";
    } else {
        return "UNKNOWN";
    }
}

AbstractGlFrameBuffer::AbstractGlFrameBuffer (GLuint fbId, glm::ivec2 dimensions) :
    m_id{fbId},
    m_dimensions{dimensions} {
    PHENYL_DASSERT(dimensions.x > 0);
    PHENYL_DASSERT(dimensions.y > 0);
}

AbstractGlFrameBuffer::AbstractGlFrameBuffer (glm::ivec2 dimensions) : m_dimensions{dimensions} {
    glGenFramebuffers(1, &m_id);
}

AbstractGlFrameBuffer::AbstractGlFrameBuffer (AbstractGlFrameBuffer&& other) noexcept :
    m_id{other.m_id},
    m_dimensions{other.m_dimensions} {
    other.m_id = -1;
}

AbstractGlFrameBuffer& AbstractGlFrameBuffer::operator= (AbstractGlFrameBuffer&& other) noexcept {
    if (m_id && m_id != -1) {
        glDeleteFramebuffers(1, &m_id);
    }

    m_id = other.m_id;
    m_dimensions = other.m_dimensions;

    other.m_id = -1;

    return *this;
}

AbstractGlFrameBuffer::~AbstractGlFrameBuffer () noexcept {
    if (m_id && m_id != -1) {
        glDeleteFramebuffers(1, &m_id);
    }
}

void AbstractGlFrameBuffer::bind () const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void AbstractGlFrameBuffer::bindViewport () const {
    bind();
    glViewport(0, 0, m_dimensions.x, m_dimensions.y);
}

void AbstractGlFrameBuffer::clear (glm::vec4 clearColor) {
    bind();
    glClearColor(clearColor.r, clearColor.b, clearColor.g, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glClearBufferfv(GL_COLOR, GL_DRAW_BUFFER0, reinterpret_cast<GLfloat*>(&clearColor));
    //
    // GLfloat depth = 1.0f;
    // glClearBufferfv(GL_DEPTH, 0, &depth);
}

glm::ivec2 AbstractGlFrameBuffer::getDimensions () const noexcept {
    return m_dimensions;
}

GlFrameBuffer::GlFrameBuffer (glm::ivec2 dimensions, const graphics::FrameBufferProperties& properties) :
    AbstractGlFrameBuffer{dimensions},
    m_colorSampler{properties.format ?
          std::optional{GlSampler{GL_TEXTURE_2D,
            graphics::TextureProperties{.format = *properties.format, .useMipmapping = false}}} :
          std::nullopt},
    m_depthSampler{properties.depthFormat ?
          std::optional{GlSampler{GL_TEXTURE_2D,
            graphics::TextureProperties{.format = graphics::ImageFormat::DEPTH24_STENCIL8, .useMipmapping = false}}} :
          std::nullopt} {
    if (m_colorSampler) {
        m_colorSampler->createEmpty2D(dimensions.x, dimensions.y);
    }

    if (m_depthSampler) {
        m_depthSampler->createEmpty2D(dimensions.x, dimensions.y);
    }

    bind();
    if (m_colorSampler) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_colorSampler->type(), m_colorSampler->id(), 0);
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (m_depthSampler) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_depthSampler->type(),
            m_depthSampler->id(), 0);
    }

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    PHENYL_LOGW_IF((status != GL_FRAMEBUFFER_COMPLETE), LOGGER, "Frame buffer creation failed with status {}: {}",
        status, GetFrameBufferStatusString(status));

    m_valid = status == GL_FRAMEBUFFER_COMPLETE;
    PHENYL_DASSERT(m_valid);
}

phenyl::graphics::ISampler* GlFrameBuffer::getSampler () noexcept {
    return m_colorSampler ? &*m_colorSampler : nullptr;
}

phenyl::graphics::ISampler* GlFrameBuffer::getDepthSampler () noexcept {
    return m_depthSampler ? &*m_depthSampler : nullptr;
}

GlWindowFrameBuffer::GlWindowFrameBuffer (glm::ivec2 dimensions) : AbstractGlFrameBuffer{0, dimensions} {}

phenyl::graphics::ISampler* GlWindowFrameBuffer::getSampler () noexcept {
    PHENYL_ABORT("Cannot sample window frame buffer!");
}

phenyl::graphics::ISampler* GlWindowFrameBuffer::getDepthSampler () noexcept {
    PHENYL_ABORT("Cannot sample window frame buffer!");
}

void GlWindowFrameBuffer::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    m_dimensions = newResolution;
}
