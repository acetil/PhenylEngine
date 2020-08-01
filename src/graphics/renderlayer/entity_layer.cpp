#include <component/main_component.h>
#include "entity_layer.h"

#include <stdio.h>


#define BUFFER_SIZE 100 * 2 * 6

#define NUM_TRIANGLE_VERTICES 3
#define TRIANGLES_PER_SPRITE 2
#define NUM_POS_PER_VERTEX 2

using namespace graphics;

void bufferPosData (component::EntityMainComponent* comp, int numEntities, int direction, Buffer* buffer);
void bufferUvData (float* uv, int numEntities, int direction, Buffer* buf);
std::string EntityRenderLayer::getName () {
    return "entity_layer";
}

int EntityRenderLayer::getPriority () {
    return 1;
}

bool graphics::EntityRenderLayer::isActive () {
    return active;
}

void EntityRenderLayer::gatherData () {

}

void EntityRenderLayer::preRender (graphics::Renderer* renderer) {
    componentManager->applyFunc(bufferPosData, 1, &buffers[0]);
    componentManager->applyFunc(bufferUvData, 2, &buffers[1]);
    numTriangles = buffers[0].currentSize() / sizeof(float) / 2 / 3;
    renderer->bufferData(buffIds, buffers);
}

int EntityRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void EntityRenderLayer::applyUniform (int uniformId, void* data) {

}

void EntityRenderLayer::applyCamera (graphics::Camera camera) {
    shaderProgram->useProgram();
    shaderProgram->appplyUniform(camera.getUniformName(), camera.getCamMatrix());
}

void EntityRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    shaderProgram->useProgram();
    frameBuf->bind();
    //printf("Num triangles: %d\n", numTriangles);
    renderer->render(buffIds, shaderProgram, numTriangles); // TODO: Make better
}

EntityRenderLayer::EntityRenderLayer (graphics::Renderer* renderer,
                                                component::ComponentManager<game::AbstractEntity*>* componentManager) {
    this->componentManager = componentManager;
    //this->shaderProgram = renderer->getProgram("entity").value();
    this->shaderProgram = renderer->getProgram("default").value(); // TODO
    this->buffIds = renderer->getBufferIds(2, BUFFER_SIZE * 2 * sizeof(float));
    this->buffers[0] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[1] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
}

void bufferPosData (component::EntityMainComponent* comp, int numEntities, int direction, Buffer* buffer) {
    for (int i = 0; i < numEntities; i++) {
        //logging::log(LEVEL_DEBUG, "Buffering entity pos data!");
        //auto ptr = buffer->getVertexBufferPos();
        float vertices[4][2];
        vertices[0][0] = comp->pos[0];
        vertices[0][1] = comp->pos[1];
        vertices[1][0] = vertices[0][0] + comp->vec1[0];
        vertices[1][1] = vertices[0][1] + comp->vec1[1];
        vertices[2][0] = vertices[0][0] + comp->vec2[0];
        vertices[2][1] = vertices[0][1] + comp->vec2[1];
        vertices[3][0] = vertices[0][0] + comp->vec1[0] + comp->vec2[0];
        vertices[3][1] = vertices[0][1] + comp->vec1[1] + comp->vec2[1];
        for (int j = 0; j < NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE; j++) {
            int correctedVertex = j % NUM_TRIANGLE_VERTICES + j / NUM_TRIANGLE_VERTICES;
            buffer->pushData(vertices[correctedVertex], 2);
        }
        comp++;
    }

}
void bufferUvData (float* uv, int numEntities, int direction, Buffer* buf) {
    for (int i = 0; i < numEntities * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE; i++) {
        buf->pushData(uv, 2);
        uv += 2;
    }
}