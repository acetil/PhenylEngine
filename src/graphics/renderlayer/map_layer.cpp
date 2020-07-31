#include "map_layer.h"

#define BUFFER_SIZE 100

using namespace graphics;


std::string MapRenderLayer::getName() {
    return "map_layer";
}

int MapRenderLayer::getPriority() {
    return 1;
}

bool MapRenderLayer::isActive() {
    return active;
}

BufferInfo MapRenderLayer::getBufferInfo() {
    if (buffer.isEmpty()) {
        return BufferInfo(2, std::vector({std::pair<int,int>(sizeof(float), 2), std::pair<int,int>(sizeof(float), 2)}),
                std::vector({BUFFER_SIZE, BUFFER_SIZE}), std::vector({true, true}));
    }
    return BufferInfo();
}

void MapRenderLayer::addBuffer(BufferNew buf) {
    buffer = std::move(buf);
}

void MapRenderLayer::gatherData () {

}

void MapRenderLayer::preRender (Renderer* renderer) {

}

int MapRenderLayer::getUniformId (std::string uniformName) {
    return 0;
}

void MapRenderLayer::applyUniform (int uniformId, void* data) {

}

void MapRenderLayer::applyCamera (Camera camera) {

}

void MapRenderLayer::render (Renderer* renderer, FrameBuffer* frameBuf) {

}

void MapRenderLayer::attachMap (game::Map* map) {
    this->map = map;
}



