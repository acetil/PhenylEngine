#include "component/main_component.h"
#include "entity_layer.h"


#define BUFFER_SIZE 100 * 2 * 6

#define NUM_TRIANGLE_VERTICES 3
//#define TRIANGLES_PER_SPRITE 2
//#define NUM_POS_PER_VERTEX 2

using namespace graphics;

void bufferPosData (FixedModel* comp, int numEntities, [[maybe_unused]] int direction, std::pair<Buffer*, AbsolutePosition*> tup);
void bufferUvData (FixedModel* comp, int numEntities, [[maybe_unused]] int direction, Buffer* buf);
void bufferActualPosData (AbsolutePosition* comp, int numEntities, [[maybe_unused]] int direction, Buffer* bufs);
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
    componentManager->applyFunc<FixedModel>(bufferPosData, std::pair(&buffers[0],
                                                            componentManager->getComponent<AbsolutePosition>()));
    componentManager->applyFunc<FixedModel>(bufferUvData, &buffers[1]);
    componentManager->applyFunc<AbsolutePosition>(bufferActualPosData, &buffers[2]);

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
    shaderProgram->applyUniform(camera.getUniformName(), camera.getCamMatrix());
}

void EntityRenderLayer::render (graphics::Renderer* renderer, graphics::FrameBuffer* frameBuf) {
    shaderProgram->useProgram();
    frameBuf->bind();
    //printf("Num triangles: %d\n", numTriangles);
    renderer->render(buffIds, shaderProgram, numTriangles); // TODO: Make better
}

EntityRenderLayer::EntityRenderLayer (graphics::Renderer* renderer,
                                                component::EntityComponentManager* componentManager) {
    this->componentManager = componentManager;
    //this->shaderProgram = renderer->getProgram("entity").value();
    this->shaderProgram = renderer->getProgram("default").value(); // TODO
    this->buffIds = renderer->getBufferIds(5, BUFFER_SIZE * 2 * sizeof(float), {2, 2, 2, 2, 2});
    this->buffers[0] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[1] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[2] = Buffer(BUFFER_SIZE * 2, sizeof(float), false);
    this->buffers[3] = Buffer(BUFFER_SIZE * 4, sizeof(float), false);
    this->buffers[4] = Buffer(BUFFER_SIZE * 4, sizeof(float), false); // TODO: better way to input matrices
}

void bufferPosData (FixedModel* comp, int numEntities, [[maybe_unused]] int direction, std::pair<Buffer*, AbsolutePosition*> tup) {
    auto [buf, modComp] = tup;
    for (int i = 0; i < numEntities; i++) {
        //logging::logf(LEVEL_DEBUG, "Num vertices: %d", comp[i].positionData.size());
        //logging::log(LEVEL_DEBUG, "Buffering entity pos data!");
        //auto ptr = buffer->getVertexBufferPos();
        /*glm::vec2 vertices[4];
        vertices[0] = comp->pos;
        vertices[1] = comp->pos + comp->vec1;
        vertices[2] = comp->pos + comp->vec2;
        vertices[3] = comp->pos + comp->vec1 + comp->vec2;
        for (int j = 0; j < NUM_TRIANGLE_VERTICES * TRIANGLES_PER_SPRITE; j++) {
            int correctedVertex = j % NUM_TRIANGLE_VERTICES + j / NUM_TRIANGLE_VERTICES;
            buffer->pushData(&vertices[correctedVertex].x, 2);
        }*/

        buf->pushData(comp->positionData.begin(), comp->positionData.size());
        modComp->vertices = comp->positionData.size() / 2;
        comp++;
        modComp++;
    }

}
void bufferUvData (FixedModel* comp, int numEntities, [[maybe_unused]] int direction, Buffer* buf) {
    for (int i = 0; i < numEntities; i++) {
        buf->pushData(comp->uvData.begin(), comp->uvData.size());
        comp++;
    }
}
void bufferActualPosData (AbsolutePosition* comp, int numEntities, [[maybe_unused]] int direction, Buffer* bufs) {
    for (int i = 0; i < numEntities; i++) {
        //logging::logf(LEVEL_DEBUG, "Abs pos: <%f, %f>", comp->pos.x, comp->pos.y);
        for (int j = 0; j < comp->vertices; j++) {
            bufs->pushData(&comp->pos[0], 2);
            (bufs + 1)->pushData(&comp->transform[0][0], 2);
            (bufs + 2)->pushData(&comp->transform[1][0], 2);
            //buf.second->pushData(&comp->transform[2], 2); // TODO: update buffer implementation
        }
        comp++;
    }
}