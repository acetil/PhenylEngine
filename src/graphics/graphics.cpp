#include <stdlib.h>
#include <utility>

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
    
    glfwSwapBuffers(window);
}
void graphics::Graphics::pollEvents () {
    glfwPollEvents();
}
void graphics::Graphics::drawTexSquare (float x, float y, float scale, int textureId) {
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
}
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
void graphics::Buffer::pushBuffer (float* posData, float* uvData) {
    if (++numSprites >= maxNumSprites) {
        reinitBuffer(maxNumSprites * 2); //TODO update to sensible values
    }
    size_t sizePosData = NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX * sizeof(float);
    size_t sizeUvData = NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX * sizeof(float);
    memcpy(vertexPosCurrent, posData, sizePosData);
    memcpy(vertexUvCurrent, uvData, sizeUvData);
    vertexPosCurrent += NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_POS_PER_VERTEX;
    vertexUvCurrent += NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE * NUM_UV_PER_VERTEX;
}
void graphics::Buffer::flushBuffer (ShaderProgram* shader, Camera* camera, TextureAtlas* textureAtlas) {
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
    numSprites = 0;
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