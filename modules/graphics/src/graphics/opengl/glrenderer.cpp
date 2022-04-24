#include "graphics/opengl/glrenderer.h"
#include "graphics/opengl/glshader.h"
#include "graphics/opengl/glpipelinestage.h"
#include "graphics/renderers/window_callbacks.h"

#include "graphics/opengl/input/glfw_key_input.h"
#include "graphics/opengl/input/glfw_mouse_input.h"

#include "glcallbacks.h"
#include "util/profiler.h"
#include <vector>
using namespace graphics;
GLRenderer::GLRenderer (GLFWwindow* window) {
    // TODO: move graphics init code here
    this->window = window;
    windowBuf = std::make_shared<GLFrameBuffer>();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    setupErrorHandling();
    util::setProfilerTimingFunction(glfwGetTime);

    keyInput = std::make_shared<GLFWKeyInput>(window);
    mouseInput = std::make_shared<GLFWMouseInput>(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

double GLRenderer::getCurrentTime () {
    return glfwGetTime();
}

bool GLRenderer::shouldClose () {
    return glfwWindowShouldClose(window);
}

void GLRenderer::pollEvents () {
    glfwPollEvents();
    keyInput->update();
    mouseInput->update();
}

void GLRenderer::clearWindow () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

FrameBuffer* GLRenderer::getWindowBuffer () {
    return windowBuf.get();
}

/*std::optional<ShaderProgram*> GLRenderer::getProgram (std::string program) {
    if (shaderPrograms.find(program) == shaderPrograms.end()) {
        return std::nullopt;
    }
    return std::optional(shaderPrograms[program]);
}*/

void GLRenderer::finishRender () {
    glfwSwapBuffers(window);
}

void GLRenderer::addShader (const std::string& name, ShaderProgram* program) {
    shaderPrograms[name] = program;
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
                //logging::logf(LEVEL_INFO, "GL info notification from %s with type %s and message %s",
                //sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_LOW:
                logging::log(LEVEL_WARNING, "GL low severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                logging::log(LEVEL_ERROR, "GL medium severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                logging::log(LEVEL_ERROR, "GL high severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
            default:
                logging::log(LEVEL_WARNING, "GL unknown severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
        }
    }, nullptr);
}

GraphicsTexture GLRenderer::loadTexture (int width, int height, unsigned char* data) {
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    logging::log(LEVEL_INFO, "Generating mipmaps for {} * {} texture atlas", width, width);
    glGenerateMipmap(GL_TEXTURE_2D);
    return GraphicsTexture(this, id);
}

void GLRenderer::bindTexture (unsigned int textureId) {
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLRenderer::setupWindowCallbacks (std::unique_ptr<WindowCallbackContext> ctx) {
    callbackCtx = std::move(ctx);
    glfwSetWindowUserPointer(window, callbackCtx.get());
    setupGLWindowCallbacks(window);
}

// TODO: make profiles for different magfilter/minfilter/mipmapping
GraphicsTexture GLRenderer::loadTextureGrey (int width, int height, unsigned char* data) {
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    // mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //logging::log(LEVEL_INFO, "Generating mipmaps for {} * {} texture atlas", width, width);
    glGenerateMipmap(GL_TEXTURE_2D);
    return GraphicsTexture(this, id);
}

void GLRenderer::invalidateWindowCallbacks () {
    glfwSetWindowUserPointer(window, nullptr);
    removeGLWindowCallbacks(window);
    callbackCtx = nullptr;
}

GLRenderer::~GLRenderer () {
    for (const auto& pair : shaderPrograms) {
        delete pair.second;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLRenderer::addShader (const std::string& shaderName, const ShaderProgramBuilder& shaderBuilder) {
    shaderProgramsNew[shaderName] = ShaderProgramNew{GLShaderProgram::NewSharedPtr(shaderBuilder)};
}

util::Optional<ShaderProgramNew> GLRenderer::getProgramNew (const std::string& program) {
    if (shaderProgramsNew.contains(program)) {
        return {shaderProgramsNew.at(program)};
    } else {
        return util::NullOpt;
    }
}

PipelineStage GLRenderer::buildPipelineStage (const PipelineStageBuilder& stageBuilder) {
    auto spec = stageBuilder.build();
    return PipelineStage{spec.shader, std::make_unique<GLPipelineStage>(spec)};
}

std::shared_ptr<RendererBufferHandle> GLRenderer::makeBufferHandle () {
    return std::make_shared<GlBuffer>();
}

std::shared_ptr<common::InputSource> GLRenderer::getMouseInput () {
    return mouseInput;
}

std::vector<std::shared_ptr<common::InputSource>> GLRenderer::getInputSources () {
    return {mouseInput, keyInput};
}





