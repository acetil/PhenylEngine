#include <component/main_component.h>
#include "entity_layer.h"

#define BUFFER_SIZE 100

#define NUM_TRIANGLE_VERTICES 3
#define TRIANGLES_PER_SPRITE 2
#define NUM_POS_PER_VERTEX 2

using namespace graphics;

void bufferPosData (component::EntityMainComponent* comp, int numEntities, int direction, BufferNew buffer);
void bufferUvData (float* uv, int numEntities, int direction, BufferNew buf);
std::string EntityRenderLayer::getName () {
    return "entity_layer";
}

int EntityRenderLayer::getPriority () {
    return 1;
}

bool graphics::EntityRenderLayer::isActive () {
    return active;
}

BufferInfo EntityRenderLayer::getBufferInfo () {
    if (numBuffers < 2) {
        return BufferInfo(2 - numBuffers, std::vector({std::pair<int,int>(sizeof(float), 2), std::pair<int,int>(sizeof(float), 2)}),
                std::vector({BUFFER_SIZE, BUFFER_SIZE}), std::vector({true, true}));
    }
    return graphics::BufferInfo();
}

void EntityRenderLayer::addBuffer (BufferNew buf) {
    buffers[numBuffers] = std::move(buf);
    numBuffers++;
}

void EntityRenderLayer::gatherData () {

}

void EntityRenderLayer::preRender (graphics::Renderer* renderer) {
    componentManager->applyFunc(bufferPosData, 1, buffers[0]);
    componentManager->applyFunc(bufferUvData, 2, buffers[1]);
}

int EntityRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void EntityRenderLayer::applyUniform (int uniformId, void* data) {

}

void EntityRenderLayer::applyCamera (graphics::CameraNew camera) {

}

void EntityRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {

}

EntityRenderLayer::EntityRenderLayer (graphics::Renderer* renderer,
                                                component::ComponentManager<game::AbstractEntity*>* componentManager) {
    this->componentManager = componentManager;
    this->shaderProgram = renderer->getProgram("entity").value();

}

void bufferPosData (component::EntityMainComponent* comp, int numEntities, int direction, BufferNew buffer) {
    for (int i = 0; i < numEntities; i++) {
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
            buffer.pushData(vertices[correctedVertex], 2);
        }
        comp++;
    }
}
void bufferUvData (float* uv, int numEntities, int direction, BufferNew buf) {
    for (int i = 0; i < numEntities * NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE; i++) {
        buf.pushData(uv, 2);
        uv += 2;
    }
}