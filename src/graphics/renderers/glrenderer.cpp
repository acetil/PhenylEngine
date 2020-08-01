#include "glrenderer.h"
#include <vector>
using namespace graphics;
GLRenderer::GLRenderer (GLFWwindow* window) {
    // TODO: move graphics init code here
    this->window = window;
    windowBuf = new GLFrameBuffer();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    setupErrorHandling();
}

double GLRenderer::getCurrentTime () {
    return glfwGetTime();
}

bool GLRenderer::shouldClose () {
    return glfwWindowShouldClose(window);
}

void GLRenderer::pollEvents () {
    glfwPollEvents();
}

void GLRenderer::clearWindow () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

FrameBuffer* GLRenderer::getWindowBuffer () {
    return windowBuf;
}

std::optional<ShaderProgram*> GLRenderer::getProgram (std::string program) {
    if (shaderPrograms.find(program) == shaderPrograms.end()) {
        return std::nullopt;
    }
    return std::optional(shaderPrograms[program]);
}

GraphicsBufferIds GLRenderer::getBufferIds (int requestedBufs, int bufferSize) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    auto buffers = new GLuint[requestedBufs];

    glGenBuffers(requestedBufs, buffers);

    for (int i = 0; i < requestedBufs; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, 0, nullptr); // TODO: update to be more generic
    }

    std::vector<GLuint> bufVec;
    bufVec.reserve(requestedBufs);
    for (int i = 0; i < requestedBufs; i++) {
        bufVec.push_back(buffers[i]);
    }
    delete[] buffers;
    return GraphicsBufferIds(vao, bufVec);

}

void GLRenderer::bufferData (GraphicsBufferIds& ids, BufferNew* buffers) {
    glBindVertexArray(ids.vaoId);
    for (int i = 0; i < ids.vboIds.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, ids.vboIds[i]);
        //printf("Current size: %d\n", buffers[i].currentSize());
        glBufferData(GL_ARRAY_BUFFER, buffers[i].currentSize(), buffers[i].getData(), GL_DYNAMIC_DRAW);
        buffers[i].clearData();
    }
}

void GLRenderer::render (GraphicsBufferIds& ids, ShaderProgram* program, int numTriangles) {
    program->useProgram();
    glBindVertexArray(ids.vaoId);
    glDrawArrays(GL_TRIANGLES, 0, numTriangles * 3);
    //logging::logf(LEVEL_DEBUG, "Rendered %d triangles!", numTriangles);
}

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
        const char* severityString;
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
                logging::logf(LEVEL_WARNING, "GL low severity message from %s with type %s and message %s",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_MEDIUM:
                logging::logf(LEVEL_ERROR, "GL medium severity message from %s with type %s and message %s",
                              sourceString, typeString, message);
                break;
            case GL_DEBUG_SEVERITY_HIGH:
                logging::logf(LEVEL_ERROR, "GL high severity message from %s with type %s and message %s",
                              sourceString, typeString, message);
            default:
                logging::logf(LEVEL_WARNING, "GL unknown severity message from %s with type %s and message %s",
                              sourceString, typeString, message);
        }
    }, NULL);
}





