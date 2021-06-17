#include "map_layer.h"

#include <utility>

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
    if (needDataBuffer) {
        auto models = map->getModels();
        int numVertices = 0;
        for (const auto& m : models) {
            numVertices += m.first.vertices;
        }
        bufferIds = renderer->getBufferIds(5, numVertices * 2 * sizeof(float), {2, 2, 2, 2, 2});
        buffers[0] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[1] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[2] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[3] = Buffer(numVertices * 2, sizeof(float), true);
        buffers[4] = Buffer(numVertices * 2, sizeof(float), true);
        for (auto& m : models) {
            buffers[0].pushData(m.second.positionData.begin(), m.second.positionData.size());
            buffers[1].pushData(m.second.uvData.begin(), m.second.uvData.size());
            for (int i = 0; i < m.first.vertices; i++) {
                buffers[2].pushData(&m.first.pos[0], 2);
                buffers[3].pushData(&m.first.transform[0][0], 2);
                buffers[4].pushData(&m.first.transform[1][0], 2);
            }
        }
        logging::log(LEVEL_DEBUG, "Sizes: {} {} {} {} {}", buffers[0].currentSize(), buffers[1].currentSize(), buffers[2].currentSize(),
                      buffers[3].currentSize(), buffers[4].currentSize());
        renderer->bufferData(bufferIds, buffers);
        needDataBuffer = false;
        numTriangles = numVertices / 3;
        logging::log(LEVEL_DEBUG, "Buffered map data with {} vertices!", numVertices);

    }
    /*if (requiresBuffer) {
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
    }*/
}

int MapRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void MapRenderLayer::applyUniform (int uniformId, void* data) {

}

void MapRenderLayer::applyCamera (Camera camera) {
    program->useProgram();
    program->applyUniform(camera.getUniformName(), camera.getCamMatrix());
}

void MapRenderLayer::render (Renderer* renderer, FrameBuffer* frameBuf) {
    program->useProgram();
    frameBuf->bind();
    renderer->render(bufferIds, program, numTriangles);
}

void MapRenderLayer::attachMap (game::Map::SharedPtr _map) {
    this->map = std::move(_map);
    active = true;
    needDataBuffer = true;
}



