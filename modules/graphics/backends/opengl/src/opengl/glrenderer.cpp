#include "glrenderer.h"

#include "util/profiler.h"
#include "core/assets/assets.h"

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
#include "resources/shaders/blinn_phong.vert.h"
#include "resources/shaders/blinn_phong.frag.h"
#include "resources/shaders/mesh_prepass.vert.h"
#include "resources/shaders/postprocess.vert.h"
#include "resources/shaders/noop_postprocess.frag.h"
#include "resources/shaders/shadow_map.vert.h"

#include "glbuffer.h"
#include "gluniform_buffer.h"
#include "glpipeline.h"
#include "glimage_texture.h"
#include "glarray_texture.h"

#include <vector>
using namespace phenyl::graphics;
using namespace phenyl::opengl;

phenyl::Logger phenyl::opengl::detail::OPENGL_LOGGER{"OPENGL", PHENYL_LOGGER};

GLRenderer::GLRenderer (std::unique_ptr<glfw::GLFWViewport> viewport) : viewport{std::move(viewport)}, windowFrameBuffer{this->viewport->getResolution()} {
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    clearColor = {0, 0, 0, 1};
    setupErrorHandling();
    util::setProfilerTimingFunction(glfwGetTime);

    shaderManager.selfRegister();

    //this->viewport->addUpdateHandler(this);
    this->viewport->addUpdateHandler(&windowFrameBuffer);
}

double GLRenderer::getCurrentTime () {
    return viewport->getTime();
}

void GLRenderer::clearWindow () {
    windowFrameBuffer.clear(clearColor);
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
                PHENYL_TRACE(detail::OPENGL_LOGGER,  "GL debug notification from {} with type {} and message {}", sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                PHENYL_LOGW(detail::OPENGL_LOGGER, "GL low severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                PHENYL_LOGE(detail::OPENGL_LOGGER, "GL medium severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                PHENYL_LOGF(detail::OPENGL_LOGGER, "GL high severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            default:
                PHENYL_LOGW(detail::OPENGL_LOGGER, "GL unknown severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
        }
    }, nullptr);
}

std::unique_ptr<IBuffer> GLRenderer::makeRendererBuffer (std::size_t startCapacity, std::size_t elementSize) {
    return std::make_unique<GlBuffer>(startCapacity, elementSize);
}

PipelineBuilder GLRenderer::buildPipeline () {
    return PipelineBuilder(std::make_unique<GlPipelineBuilder>(&windowFrameBuffer));
}

std::unique_ptr<IUniformBuffer> GLRenderer::makeRendererUniformBuffer (bool readable) {
    return std::move(std::make_unique<GlUniformBuffer>(readable));
}

void GLRenderer::loadDefaultShaders () {
    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual box shader!");
    boxShader = core::Assets::LoadVirtual("phenyl/shaders/box", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_BOX_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_BOX_FRAGMENT_FRAG)
            .withAttrib(ShaderDataType::VEC2F, "pos")
            .withAttrib(ShaderDataType::VEC2F, "rectPosIn")
            .withAttrib(ShaderDataType::VEC4F, "borderColourIn")
            .withAttrib(ShaderDataType::VEC4F, "bgColourIn")
            .withAttrib(ShaderDataType::VEC4F, "boxDetailIn")
            .withUniformBlock("Uniform")
            .build()
    });


    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual debug shader!");
    debugShader = core::Assets::LoadVirtual("phenyl/shaders/debug", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_DEBUG_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_DEBUG_FRAGMENT_FRAG)
            .withAttrib(ShaderDataType::VEC3F, "position")
            .withAttrib(ShaderDataType::VEC4F, "colourOut")
            .withUniformBlock("Uniform")
            .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual sprite shader!");
    spriteShader = core::Assets::LoadVirtual("phenyl/shaders/sprite", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_SPRITE_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_SPRITE_FRAGMENT_FRAG)
            .withAttrib(ShaderDataType::VEC2F, "position")
            .withAttrib(ShaderDataType::VEC2F, "uvOut")
            .withUniformBlock("Camera")
            .withSampler("textureSampler")
            .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual canvas shader!");
    textShader = core::Assets::LoadVirtual("phenyl/shaders/canvas", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_CANVAS_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_CANVAS_FRAGMENT_FRAG)
            .withAttrib(ShaderDataType::VEC2F, "pos")
            .withAttrib(ShaderDataType::VEC3F, "uvOut")
            .withAttrib(ShaderDataType::VEC4F, "colorOut")
            .withUniformBlock("Uniform")
            .withSampler("textureSampler")
            .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual particle shader!");
    particleShader = core::Assets::LoadVirtual("phenyl/shaders/particle", Shader{GlShader::Builder()
            .withSource(ShaderSourceType::VERTEX, EMBED_PARTICLE_VERTEX_VERT)
            .withSource(ShaderSourceType::FRAGMENT, EMBED_PARTICLE_FRAGMENT_FRAG)
            .withAttrib(ShaderDataType::VEC2F, "pos")
            .withAttrib(ShaderDataType::VEC4F, "colourIn")
            .withUniformBlock("Camera")
            .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual Blinn-Phong shader!");
    meshShader = core::Assets::LoadVirtual("phenyl/shaders/blinn_phong", Shader{GlShader::Builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_BLINN_PHONG_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_BLINN_PHONG_FRAG)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::VEC3F, "normal")
        .withAttrib(ShaderDataType::VEC2F, "texcoord_0")
        .withAttrib(ShaderDataType::MAT4F, "model")
        .withUniformBlock("GlobalUniform")
        .withUniformBlock("BPLightUniform")
        .withUniformBlock("Material")
        .withSampler("ShadowMap")
        .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual shadow mapping shader!");
    shadowMapShader = core::Assets::LoadVirtual("phenyl/shaders/shadow_map", Shader{GlShader::Builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_SHADOW_MAP_VERT)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::MAT4F, "model")
        .withUniformBlock("BPLightUniform")
        .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual mesh z-prepass shader!");
    prepassShader = core::Assets::LoadVirtual("phenyl/shaders/mesh_prepass", Shader{GlShader::Builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_MESH_PREPASS_VERT)
        .withAttrib(ShaderDataType::VEC3F, "position")
        .withAttrib(ShaderDataType::MAT4F, "model")
        .withUniformBlock("GlobalUniform")
        .build()
    });

    PHENYL_TRACE(detail::OPENGL_LOGGER, "Loading virtual no-op post-process shader!");
    noopPostShader = core::Assets::LoadVirtual("phenyl/shaders/postprocess/noop", Shader{GlShader::Builder()
        .withSource(ShaderSourceType::VERTEX, EMBED_POSTPROCESS_VERT)
        .withSource(ShaderSourceType::FRAGMENT, EMBED_NOOP_POSTPROCESS_FRAG)
        .withAttrib(ShaderDataType::VEC2F, "position")
        .withSampler("frameBuffer")
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
    auto viewport = std::make_unique<glfw::GLFWViewport>(properties);
    PHENYL_ASSERT_MSG(viewport, "Failed to initialise GLFW viewport!");

    return std::make_unique<GLRenderer>(std::move(viewport));
}

void GLRenderer::render () {
    clearWindow();
    layerRender();
    viewport->swapBuffers();
}

std::unique_ptr<IImageTexture> GLRenderer::makeRendererImageTexture (const TextureProperties& properties) {
    return std::make_unique<GlImageTexture>(properties);
}

std::unique_ptr<IImageArrayTexture> GLRenderer::makeRendererArrayTexture (const TextureProperties& properties, std::uint32_t width, std::uint32_t height) {
    return std::make_unique<GlArrayTexture>(properties, width, height);
}

std::unique_ptr<IFrameBuffer> GLRenderer::makeRendererFrameBuffer (const FrameBufferProperties& properties,
    std::uint32_t width, std::uint32_t height) {
    return std::make_unique<GlFrameBuffer>(glm::ivec2{width, height}, properties);
}

void GLRenderer::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    //glViewport(0, 0, newResolution.x, newResolution.y);
}

std::unique_ptr<Renderer> phenyl::graphics::MakeGLRenderer (const phenyl::graphics::GraphicsProperties& properties) {
    return GLRenderer::Make(properties);
}
