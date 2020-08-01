#include "map_layer.h"

#define BUFFER_SIZE 100

using namespace graphics;


MapRenderLayer::MapRenderLayer (Renderer* renderer) {
    map = nullptr;
    numTriangles = 0;
    program = renderer->getProgram("default").value();
}

std::string MapRenderLayer::getName() {
    return "map_layer";
}

int MapRenderLayer::getPriority() {
    return 1;
}

bool MapRenderLayer::isActive() {
    return active;
}

void MapRenderLayer::gatherData () {
    
}

void MapRenderLayer::preRender (Renderer* renderer) {
    if (requiresBuffer) {
        bufferIds = renderer->getBufferIds(2, map->getNumTileVertices() * 6 * sizeof(float));
        requiresBuffer = false;
    }
    if (needDataBuffer) {
        int numVertices = map->getNumTileVertices();
        buffers[0] = Buffer(numVertices * 2 , sizeof(float), true);
        buffers[1] = Buffer(numVertices * 2, sizeof(float), true);
        float* vertexData = map->getTileVertices();
        float* uvData = map->getTileUvs();
        buffers[0].pushData(vertexData, numVertices * 2);
        buffers[1].pushData(uvData, numVertices * 2);
        renderer->bufferData(bufferIds, buffers);
        needDataBuffer = false;
        numTriangles = numVertices / 3;
        delete[] vertexData;
        delete[] uvData;
        logging::logf(LEVEL_DEBUG, "Buffered map data with %d vertices!", numVertices);
    }
}

int MapRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void MapRenderLayer::applyUniform (int uniformId, void* data) {

}

void MapRenderLayer::applyCamera (Camera camera) {
    program->useProgram();
    program->appplyUniform(camera.getUniformName(), camera.getCamMatrix());
}

void MapRenderLayer::render (Renderer* renderer, FrameBuffer* frameBuf) {
    program->useProgram();
    frameBuf->bind();
    renderer->render(bufferIds, program, numTriangles);
}

void MapRenderLayer::attachMap (game::Map* map) {
    this->map = map;
    active = true;
    needDataBuffer = true;
}



