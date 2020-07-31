#include "glrenderer.h"
#include <vector>
using namespace graphics;
GLRenderer::GLRenderer (GLFWwindow* window) {
    // TODO: move graphics init code here
    this->window = window;
    windowBuf = new GLFrameBuffer();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

void GLRenderer::bufferData (GraphicsBufferIds ids, BufferNew* buffers) {
    glBindVertexArray(ids.vaoId);
    for (int i = 0; i < ids.vboIds.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, ids.vboIds[i]);
        glBufferData(GL_ARRAY_BUFFER, buffers[i].currentSize(), buffers[i].getData(), GL_DYNAMIC_DRAW);
    }
}

void GLRenderer::render (GraphicsBufferIds ids, ShaderProgram* program, int numTriangles) {
    program->useProgram();
    glBindVertexArray(ids.vaoId);
    glDrawArrays(GL_TRIANGLES, 0, numTriangles * 3);
}

void GLRenderer::finishRender () {
    glfwSwapBuffers(window);
}





