#include "graphics/opengl/glrenderer.h"
#include "graphics/opengl/glshader.h"
#include "graphics/opengl/glpipelinestage.h"

#include "graphics/opengl/input/glfw_key_input.h"
#include "graphics/opengl/input/glfw_mouse_input.h"

#include "glcallbacks.h"
#include "util/profiler.h"
#include "common/assets/assets.h"

#include "resources/shaders/box_vertex.vert.h"
#include "resources/shaders/box_fragment.frag.h"
#include "resources/shaders/debug_vertex.vert.h"
#include "resources/shaders/debug_fragment.frag.h"
#include "resources/shaders/sprite_vertex.vert.h"
#include "resources/shaders/sprite_fragment.frag.h"
#include "resources/shaders/text_vertex.vert.h"
#include "resources/shaders/text_fragment.frag.h"
#include "resources/shaders/particle_vertex.vert.h"
#include "resources/shaders/particle_fragment.frag.h"

#include <vector>
using namespace phenyl::graphics;

static phenyl::Logger LOGGER{"GL_RENDERER"};

GLRenderer::GLRenderer (GLFWwindow* window) : shaderManager{this} {
    // TODO: move graphics init code here
    this->window = window;
    windowBuf = std::make_shared<GLFrameBuffer>();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    setupErrorHandling();
    util::setProfilerTimingFunction(glfwGetTime);

    //keyInput = std::make_shared<GLFWKeyInput>(window);
    keyInput = std::make_shared<GLFWKeyInput2>();
    //mouseInput = std::make_shared<GLFWMouseInput>(window);
    mouseInput = std::make_shared<GLFWMouseInput2>();
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    screenSize = {width, height};
    shaderManager.selfRegister();
}

double GLRenderer::getCurrentTime () {
    return glfwGetTime();
}

bool GLRenderer::shouldClose () {
    return glfwWindowShouldClose(window);
}

void GLRenderer::pollEvents () {
    glfwPollEvents();
    //keyInput->update();
    //mouseInput->update();
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
                PHENYL_LOGT(LOGGER,  "GL debug notification from {} with type {} and message {}", sourceString, typeString, message);
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
                PHENYL_LOGE(LOGGER, "GL high severity message from {} with type {} and message {}",
                              sourceString, typeString, message);
            default:
                PHENYL_LOGW(LOGGER, "GL unknown severity message from {} with type {} and message {}",
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
    PHENYL_LOGD(LOGGER, "Generating mipmaps for {} * {} texture", width, width);
    glGenerateMipmap(GL_TEXTURE_2D);
    return GraphicsTexture(this, id);
}

void GLRenderer::reloadTexture (unsigned int textureId, int width, int height, unsigned char* data) {
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // mipmapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    PHENYL_LOGD(LOGGER, "Generating mipmaps for {} * {} texture atlas", width, width);
    glGenerateMipmap(GL_TEXTURE_2D);
}


void GLRenderer::bindTexture (unsigned int textureId) {
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLRenderer::destroyTexture (unsigned int textureId) {
    glDeleteTextures(1, &textureId);
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
    windowCallbackCtx = nullptr;
}

GLRenderer::~GLRenderer () {
    glfwDestroyWindow(window);
    glfwTerminate();
}

/*void GLRenderer::addShader (const std::string& shaderName, const ShaderBuilder& shaderBuilder) {
    shaderProgramsNew[shaderName] = Shader{GLShaderProgram::NewSharedPtr(shaderBuilder)};
}

util::Optional<Shader> GLRenderer::getProgramNew (const std::string& program) {
    if (shaderProgramsNew.contains(program)) {
        return {shaderProgramsNew.at(program)};
    } else {
        return util::NullOpt;
    }
}*/

PipelineStage GLRenderer::buildPipelineStage (PipelineStageBuilder& stageBuilder) {
    auto spec = stageBuilder.build();
    return PipelineStage{spec.shader, std::make_unique<GLPipelineStage>(spec)};
}

std::shared_ptr<RendererBufferHandle> GLRenderer::makeBufferHandle () {
    return std::make_shared<GlBuffer>();
}

std::shared_ptr<phenyl::common::InputSource> GLRenderer::getMouseInput () {
    return mouseInput;
}

std::vector<std::shared_ptr<phenyl::common::InputSource>> GLRenderer::getInputSources () {
    return {mouseInput, keyInput};
}

void GLRenderer::setupCallbacks () {
    windowCallbackCtx = std::make_unique<GLWindowCallbackCtx>(this);
    setupGLWindowCallbacks(window, windowCallbackCtx.get());
}

glm::vec2 GLRenderer::getScreenSize () const {
    return screenSize;
}

void GLRenderer::setScreenSize (glm::vec2 _screenSize) {
    screenSize = _screenSize;
}

void GLRenderer::onKeyChange (int key, int scancode, int action, int mods) {
    keyInput->onButtonChange(scancode, action, mods);
}

void GLRenderer::onMouseButtonChange (int button, int action, int mods) {
    mouseInput->onButtonChange(button, action, mods);
}

bool GLRenderer::mouseDown () const {
    return mouseInput->getStateNum(GLFW_MOUSE_BUTTON_LEFT);
}

void GLRenderer::onMousePosChange (glm::vec2 newPos) {
    mousePos = newPos;
}

glm::vec2 GLRenderer::getMousePos () const {
    return mousePos;
}

void GLRenderer::loadDefaultShaders () {
    PHENYL_LOGD(LOGGER, "Loading virtual box shader!");
    boxShader = common::Assets::LoadVirtual("phenyl/shaders/box", Shader{std::make_shared<GLShaderProgram>(
            ShaderSourceBuilder{EMBED_BOX_VERTEX_VERT, EMBED_BOX_FRAGMENT_FRAG}
                    .addUniform<glm::vec2>("screenSize")
                    .build()
    )});


   PHENYL_LOGD(LOGGER, "Loading virtual debug shader!");
    debugShader = common::Assets::LoadVirtual("phenyl/shaders/debug", Shader{std::make_shared<GLShaderProgram>(
            ShaderSourceBuilder{EMBED_DEBUG_VERTEX_VERT, EMBED_DEBUG_FRAGMENT_FRAG}
                    .addUniform<glm::mat4>("camera")
                    .addUniform<glm::mat4>("screenTransform")
                    .build()
    )});

    PHENYL_LOGD(LOGGER, "Loading virtual sprite shader!");
    spriteShader = common::Assets::LoadVirtual("phenyl/shaders/sprite", Shader{std::make_shared<GLShaderProgram>(
            ShaderSourceBuilder{EMBED_SPRITE_VERTEX_VERT, EMBED_SPRITE_FRAGMENT_FRAG}
                    .addUniform<glm::mat4>("camera")
                    .build()
    )});

    PHENYL_LOGD(LOGGER, "Loading virtual text shader!");
    textShader = common::Assets::LoadVirtual("phenyl/shaders/text", Shader{std::make_shared<GLShaderProgram>(
            ShaderSourceBuilder{EMBED_TEXT_VERTEX_VERT, EMBED_TEXT_FRAGMENT_FRAG}
                    .addUniform<glm::mat4>("camera")
                    .build()
    )});

    PHENYL_LOGD(LOGGER, "Loading virtual particle shader!");
    particleShader = common::Assets::LoadVirtual("phenyl/shaders/particle", Shader{std::make_shared<GLShaderProgram>(
            ShaderSourceBuilder{EMBED_PARTICLE_VERTEX_VERT, EMBED_PARTICLE_FRAGMENT_FRAG}
                    .addUniform<glm::mat4>("camera")
                    .build()
    )});
}





