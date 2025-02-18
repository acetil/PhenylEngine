#include "glframebuffer.h"

#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_FRAME_BUFFER", detail::GRAPHICS_LOGGER};

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

AbstractGlFrameBuffer::AbstractGlFrameBuffer (GLuint fbId, glm::ivec2 dimensions) : fbId{fbId}, dimensions{dimensions} {
    PHENYL_DASSERT(dimensions.x > 0);
    PHENYL_DASSERT(dimensions.y > 0);
}

AbstractGlFrameBuffer::AbstractGlFrameBuffer (glm::ivec2 dimensions) : dimensions{dimensions} {
    glGenFramebuffers(1, &fbId);
}

AbstractGlFrameBuffer::AbstractGlFrameBuffer (AbstractGlFrameBuffer&& other) noexcept : fbId{other.fbId}, dimensions{other.dimensions} {
    other.fbId = -1;
}

AbstractGlFrameBuffer& AbstractGlFrameBuffer::operator= (AbstractGlFrameBuffer&& other) noexcept {
    if (fbId && fbId != -1) {
        glDeleteFramebuffers(1, &fbId);
    }

    fbId = other.fbId;
    dimensions = other.dimensions;

    other.fbId = -1;

    return *this;
}

AbstractGlFrameBuffer::~AbstractGlFrameBuffer () noexcept {
    if (fbId && fbId != -1) {
        glDeleteFramebuffers(1, &fbId);
    }
}

void AbstractGlFrameBuffer::bind () const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbId);
}

void AbstractGlFrameBuffer::bindViewport () const {
    bind();
    glViewport(0, 0, dimensions.x, dimensions.y);
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
    return dimensions;
}

GlFrameBuffer::GlFrameBuffer (glm::ivec2 dimensions, const FrameBufferProperties& properties) : AbstractGlFrameBuffer{dimensions}, colorSampler{GL_TEXTURE_2D, TextureProperties{.format = properties.format, .useMipmapping = false}}, depthSampler{GL_TEXTURE_2D, TextureProperties{.format = ImageFormat::DEPTH24_STENCIL8, .useMipmapping = false}} {
    colorSampler.createEmpty2D(dimensions.x, dimensions.y);
    depthSampler.createEmpty2D(dimensions.x, dimensions.y);

    bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorSampler.type(), colorSampler.id(), 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthSampler.type(), depthSampler.id(), 0);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    PHENYL_LOGW_IF((status != GL_FRAMEBUFFER_COMPLETE), LOGGER, "Frame buffer creation failed with status {}: {}", status, GetFrameBufferStatusString(status));

    valid = status == GL_FRAMEBUFFER_COMPLETE;
    PHENYL_DASSERT(valid);
}
const ISampler& GlFrameBuffer::getSampler () const noexcept {
    return colorSampler;
}

GlWindowFrameBuffer::GlWindowFrameBuffer (glm::ivec2 dimensions) : AbstractGlFrameBuffer{0, dimensions} {}

const ISampler& GlWindowFrameBuffer::getSampler () const noexcept {
    PHENYL_ABORT("Cannot sample window frame buffer!");
}

void GlWindowFrameBuffer::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    dimensions = newResolution;
}
