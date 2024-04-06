#include "graphics/opengl/glrenderer.h"

#include "util/profiler.h"
#include "common/assets/assets.h"

#include "resources/shaders/box_vertex.vert.h"
#include "resources/shaders/box_fragment.frag.h"
#include "resources/shaders/debug_vertex.vert.h"
#include "resources/shaders/debug_fragment.frag.h"
#include "resources/shaders/sprite_vertex.vert.h"
#include "resources/shaders/sprite_fragment.frag.h"
#include "resources/shaders/canvas_vertex.vert.h"
#include "resources/shaders/canvas_fragment.frag.h"
#include "resources/shaders/particle_vertex.vert.h"
#include "resources/shaders/particle_fragment.frag.h"
#include "glbuffer.h"
#include "gluniform_buffer.h"
#include "glpipeline.h"
#include "glimage_texture.h"
#include "glarray_texture.h"

#include <vector>
using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_RENDERER", detail::GRAPHICS_LOGGER};

GLRenderer::GLRenderer (std::unique_ptr<GLFWViewport> viewport) : viewport{std::move(viewport)} {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    setupErrorHandling();
    util::setProfilerTimingFunction(glfwGetTime);

    shaderManager.selfRegister();
}

double GLRenderer::getCurrentTime () {
    return viewport->getTime();
}

void GLRenderer::clearWindow () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::finishRender () {
    viewport->swapBuffers();
}

void GLRenderer::setupErrorHandling () {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([] (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
                               const void* userParam){
        const char* sourceString;
        const char* typeString;
        //const char* severityString;
        switch (source) {
            case GL_DEBUG_SOURCE_API:
                sourceString = "API";
                break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                sourceString = "WINDOW SYSTEM";
                break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                sourceString = "SHADER COMPILER";
                break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                sourceString = "THIRD PARTY";
                break;
            case GL_DEBUG_SOURCE_APPLICATION:
                sourceString = "APPLICATION";
                break;
            default:
                sourceString = "UNKNOWN";
        }
        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
                typeString = "ERROR";
                break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                typeString = "DEPRECATED BEHAVIOUR";
                break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                typeString = "UNDEFINED BEHAVIOUR";
                break;
            case GL_DEBUG_TYPE_PORTABILITY:
                typeString = "PORTABILITY";
                break;
            case GL_DEBUG_TYPE_PERFORMANCE:
                typeString = "PERFORMANCE";
                break;
            case GL_DEBUG_TYPE_OTHER:
                typeString = "OTHER";
                break;
            case GL_DEBUG_TYPE_MARKER:
                typeString = "MARKER";
                break;
            default:
                typeString = "UNKNOWN";
                break;
        }
        switch(severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                PHENYL_TRACE(LOGGER,  "GL debug notification from {} with type {} and message {}", sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                PHENYL_LOGW(LOGGER, "GL low severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                PHENYL_LOGE(LOGGER, "GL medium severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                PHENYL_LOGF(LOGGER, "GL high severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            default:
                PHENYL_LOGW(LOGGER, "GL unknown severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
        }
    }, nullptr);
}

std::unique_ptr<IBuffer> GLRenderer::makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize) {
    return std::make_unique<GlBuffer>(startCapacity, elementSize);
}

PipelineBuilder GLRenderer::buildPipeline () {
    return PipelineBuilder(std::make_unique<GlPipelineBuilder>());
}

std::unique_ptr<IUniformBuffer> GLRenderer::makeRendererUniformBuffer (bool readable) {
    return std::move(std::make_unique<GlUniformBuffer>(readable));
}

void GLRenderer::loadDefaultShaders () {
    PHENYL_TRACE(LOGGER, "Loading virtual box shader!");
    boxShader = common::Assets::LoadVirtual("phenyl/shaders/box", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_BOX_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_BOX_FRAGMENT_FRAG)
            .withUniformBlock("Uniform")
            .build()
    });


    PHENYL_TRACE(LOGGER, "Loading virtual debug shader!");
    debugShader = common::Assets::LoadVirtual("phenyl/shaders/debug", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_DEBUG_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_DEBUG_FRAGMENT_FRAG)
            .withUniformBlock("Uniform")
            .build()
    });

    PHENYL_TRACE(LOGGER, "Loading virtual sprite shader!");
    spriteShader = common::Assets::LoadVirtual("phenyl/shaders/sprite", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_SPRITE_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_SPRITE_FRAGMENT_FRAG)
            .withUniformBlock("Camera")
            .withSampler("textureSampler")
            .build()
    });

    PHENYL_TRACE(LOGGER, "Loading virtual canvas shader!");
    textShader = common::Assets::LoadVirtual("phenyl/shaders/canvas", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_CANVAS_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_CANVAS_FRAGMENT_FRAG)
            .withUniformBlock("Uniform")
            .withSampler("textureSampler")
            .build()
    });

    PHENYL_TRACE(LOGGER, "Loading virtual particle shader!");
    particleShader = common::Assets::LoadVirtual("phenyl/shaders/particle", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_PARTICLE_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_PARTICLE_FRAGMENT_FRAG)
            .withUniformBlock("Camera")
            .build()
    });
}

std::string_view GLRenderer::getName () const noexcept {
    return "GLRenderer";
}

Viewport& GLRenderer::getViewport () {
    return *viewport;
}

const Viewport& GLRenderer::getViewport () const {
    return *viewport;
}

std::unique_ptr<GLRenderer> GLRenderer::Make (const GraphicsProperties& properties) {
    auto viewport = std::make_unique<GLFWViewport>(properties);
    PHENYL_ASSERT_MSG(viewport, "Failed to initialise GLFW viewport!");

    return std::make_unique<GLRenderer>(std::move(viewport));
}

void GLRenderer::render () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    layerRender();
    viewport->swapBuffers();
}

std::unique_ptr<IImageTexture> GLRenderer::makeRendererImageTexture (const TextureProperties& properties) {
    return std::make_unique<GlImageTexture>(properties);
}

std::unique_ptr<IImageArrayTexture> GLRenderer::makeRendererArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) {
    return std::make_unique<GlArrayTexture>(properties, width, height);
}
