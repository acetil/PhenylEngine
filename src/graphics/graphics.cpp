#include <stdlib.h>
#include <utility>
#include <chrono>
#include <thread>
#include <algorithm>
#include <math.h>
#include "graphics.h"
#include "graphics_headers.h"
#include "camera.h"
#include "textures/texture_atlas.h"
#include "logging/logging.h"
using namespace graphics;

#define NUM_TRIANGLE_VERTICES 3
#define TRIANGLES_PER_SPRITE 2
#define NUM_POS_PER_VERTEX 2
#define NUM_UV_PER_VERTEX 2
// TODO: refactor into different files
// TODO: just refactor in general

graphics::Graphics::Graphics (GLFWwindow* window) {
    this->window = window;
    shaderMap = std::unordered_map<std::string, graphics::ShaderProgram*>();
    spriteBuffer = new Buffer();
    spriteBuffer->initBuffer(100);
    camera = new Camera ();
}
graphics::Graphics::~Graphics () {
    delete spriteBuffer;
    delete camera;
    delete spriteAtlas;
    for (std::pair<std::string, ShaderProgram*> programPair : shaderMap) {
        delete programPair.second;
    }
}
bool graphics::Graphics::shouldClose () {
    return window == NULL || glfwWindowShouldClose(window) != 0;
}
void graphics::Graphics::render () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: add render code
    spriteBuffer->flushBuffer(currentSpriteShader, camera, spriteAtlas);
    renderStaticData();
    glfwSwapBuffers(window);
}
void graphics::Graphics::renderStaticData () {
    // TODO: refactor and optimise to only apply/bind when necessary
    for (auto data : staticData) {
        data->shaderProgram->useProgram();
        data->shaderProgram->appplyUniform(camera->getUniformName(), camera->getCamMatrix());
        spriteAtlas->bindTextureAtlas();

        glDrawArrays(GL_TRIANGLES, 0, data->numVertices);
    }
}
void graphics::Graphics::pollEvents () {
    glfwPollEvents();
}
/*void graphics::Graphics::drawTexSquare (float x, float y, float scale, int textureId) {
    float* posData = new float[NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX];
    float* posDataPtr = posData;
    for (int i = 0; i < NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE; i++) {
        // if order is 0 1
        //             2 3
        // then vertex order goes 0,1,2,1,2,3
        int correctedVertex = i % NUM_TRIANGLE_VERTICES + i / NUM_TRIANGLE_VERTICES;
        float vertexX = x;
        float vertexY = y;
        if (correctedVertex < 2) {
            vertexY -= 1.0f * scale;;
        } else {
            vertexY += 1.0f * scale;
        }
        if (correctedVertex % 2 == 0) {
            vertexX -= 1.0f * scale;
        } else {
            vertexX += 1.0f * scale;
        }
        *(posDataPtr++) = vertexX;
        *(posDataPtr++) = vertexY;
    }
    Texture* tex = spriteAtlas->getTexture(textureId);
    spriteBuffer->pushBuffer(posData, tex->getTexUvs());
    delete[] posData;
}*/
void graphics::Graphics::addShader (std::string name, ShaderProgram* shader) {
    shaderMap[name] = shader;
}
void graphics::Graphics::setCurrentSpriteShader (std::string name) {
    currentSpriteShader = shaderMap[name];
}
void graphics::Graphics::initSpriteAtlas (std::vector<Image*> images) {
    spriteAtlas = new TextureAtlas;
    spriteAtlas->createAtlas(images);
    spriteAtlas->loadTextureAtlas();
}
int graphics::Graphics::getSpriteTextureId (std::string name) {
    return spriteAtlas->getTextureId(name);
}
void graphics::Graphics::initBuffer (unsigned int numSprites) {
    // TODO: look at and see if needs changing
    spriteBuffer->initBuffer(numSprites);
}
void graphics::Graphics::startTimer (int fps) {
    this->fps = fps;
    lastTime = glfwGetTime();
}
float graphics::Graphics::getDeltaTime () {
    while(glfwGetTime() - lastTime < 1.0 / fps) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    double currentTime = glfwGetTime();
    float deltaTime = (float)(currentTime - lastTime);
    lastTime = currentTime;
    return deltaTime;
}
StaticData* graphics::Graphics::loadStaticData (float* vertexData, float* uvData, int numVertices, int sizeVertex, int sizeUv,
    std::string shader) {
    // size is number of floats, not size in bytes
    // TODO: change to immutable loading
    StaticData* data = new StaticData;
    glGenBuffers(1, &(data->vertexVBO));
    glBindBuffer(GL_ARRAY_BUFFER, data->vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeVertex, vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, NUM_POS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &(data->uvVBO));
    glBindBuffer(GL_ARRAY_BUFFER, data->uvVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeUv, uvData, GL_STATIC_DRAW);
    glVertexAttribPointer(1, NUM_UV_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

    data->numVertices = numVertices;
    data->vertexComp = sizeVertex / numVertices;
    data->uvComp = sizeUv / numVertices;
    data->shaderProgram = shaderMap[shader];
    staticData.push_back(data);
    return data;
}
void graphics::Graphics::unloadStaticData (StaticData* data) {
    if (data = NULL) {
        logging::log(LEVEL_WARNING, "Unload requested for null static data!");
        return;
    }
    staticData.erase(std::find(staticData.begin(), staticData.end(), data));
    glDeleteBuffers(1, &(data->vertexVBO));
    glDeleteBuffers(1, &(data->uvVBO));
    delete data;
}
void graphics::Graphics::setupErrorHandling () {
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
void graphics::Buffer::initBuffer (unsigned int numSprites) {
    // inits buffer for pos and uv data, intended for glSubBuffer()
    // MUST USE malloc as otherwise you cannot realloc data if required
    // TODO: see if malloc is required
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);
    maxNumSprites = numSprites;
    posBufferSize = numSprites * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX * sizeof(float);
    uvBufferSize = numSprites * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX * sizeof(float);

    vertexPosData = (float*)malloc(posBufferSize);
    vertexUvData = (float*)malloc(uvBufferSize);
    vertexPosCurrent = vertexPosData;
    vertexUvCurrent = vertexUvData;
    glGenBuffers(1, &posBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
    glBufferData(GL_ARRAY_BUFFER, posBufferSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); // TODO: update to better match layout for in position
    glVertexAttribPointer(0, NUM_POS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &uvBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
    glBufferData(GL_ARRAY_BUFFER, uvBufferSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, NUM_UV_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);
}

void graphics::Buffer::reinitBuffer (unsigned int numSprites) {
    // only use if things go wrong!
    logging::logf(LEVEL_WARNING, "Forced to reallocate buffer with %u sprites!", numSprites);
    maxNumSprites = numSprites;
    posBufferSize = numSprites * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX * sizeof(float);
    uvBufferSize = numSprites * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX * sizeof(float);

    vertexPosData = (float*)realloc(vertexPosData, posBufferSize);
    vertexUvData = (float*)realloc(vertexUvData, uvBufferSize);
    vertexPosCurrent = vertexPosData;
    vertexUvCurrent = vertexUvData;

    glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
    glBufferData(GL_ARRAY_BUFFER, posBufferSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); // TODO: update to better match layout for in position
    glVertexAttribPointer(0, NUM_POS_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
    glBufferData(GL_ARRAY_BUFFER, uvBufferSize, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, NUM_UV_PER_VERTEX, GL_FLOAT, GL_FALSE, 0, NULL);
}
/*void graphics::Buffer::pushBuffer (float* posData, float* uvData) {
    if (++numSprites >= maxNumSprites) {
        reinitBuffer(maxNumSprites * 2); //TODO update to sensible values
    }
    size_t sizePosData = NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX * sizeof(float);
    size_t sizeUvData = NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX * sizeof(float);
    memcpy(vertexPosCurrent, posData, sizePosData);
    memcpy(vertexUvCurrent, uvData, sizeUvData);
    vertexPosCurrent += NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX;
    vertexUvCurrent += NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX;
}*/
void graphics::Buffer::flushBuffer (ShaderProgram* shader, Camera* camera, TextureAtlas* textureAtlas) {
    unsigned int numSprites = fmin(numSpritesVertex, numSpritesUv);
    shader->useProgram();
    shader->appplyUniform(camera->getUniformName(), camera->getCamMatrix());
    textureAtlas->bindTextureAtlas();
    glBindBuffer(GL_ARRAY_BUFFER, posBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numSprites * TRIANGLES_PER_SPRITE * 
        NUM_TRIANGLE_VERTICES * NUM_POS_PER_VERTEX * sizeof(float), vertexPosData);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numSprites * TRIANGLES_PER_SPRITE * 
        NUM_TRIANGLE_VERTICES * NUM_UV_PER_VERTEX * sizeof(float), vertexUvData);

    glDrawArrays(GL_TRIANGLES, 0, numSprites * TRIANGLES_PER_SPRITE * NUM_TRIANGLE_VERTICES);
    numSpritesUv = 0;
    numSpritesVertex = 0;
    vertexPosCurrent = vertexPosData;
    vertexUvCurrent = vertexUvData;
}
graphics::Buffer::~Buffer () {
    if (vertexPosData != NULL) {
        free(vertexPosData);
    }
    if (vertexUvData != NULL) {
        free(vertexPosData);
    }
}

void Graphics::bufferEntityPositions (component::EntityMainComponent* comp, int numEntities, int direction, Buffer* buffer) {
    for (int i = 0; i < numEntities; i++) {
        auto ptr = buffer->getVertexBufferPos();
        float vertices[4][2];
        vertices[0][0] = comp->pos[0];
        vertices[0][1] = comp->pos[1];
        vertices[1][0] = vertices[0][0] + comp->vec1[0];
        vertices[1][1] = vertices[0][1] + comp->vec1[1];
        vertices[2][0] = vertices[0][0] + comp->vec2[0];
        vertices[2][1] = vertices[0][1] + comp->vec2[1];
        vertices[3][0] = vertices[0][0] + comp->vec1[0] + comp->vec2[0];
        vertices[3][1] = vertices[0][1] + comp->vec1[1] + comp->vec2[1];
        for (int j = 0; j < NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX; j++) {
            int correctedVertex = j % NUM_TRIANGLE_VERTICES + j / NUM_TRIANGLE_VERTICES;
            ptr[2 * j] = vertices[correctedVertex][0];
            ptr[2 * j + 1] = vertices[correctedVertex][1];
        }
    }
}
float* Buffer::getVertexBufferPos () {
    float* original = vertexPosCurrent;
    vertexPosCurrent += NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX;
    numSpritesVertex++;
    return original;
}
float* Buffer::getUvBufferPos () {
    float* original = vertexUvCurrent;
    vertexUvCurrent += NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX;
    numSpritesUv++;
    return original;
}
Buffer* Graphics::getSpriteBuffer () {
    return spriteBuffer;
}
TextureAtlas* Graphics::getTextureAtlas () {
    return spriteAtlas;
}