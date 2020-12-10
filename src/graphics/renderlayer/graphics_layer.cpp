#include "graphics_layer.h"
#include "map_layer.h"
#include "entity_layer.h"

using namespace graphics;

GraphicsRenderLayer::GraphicsRenderLayer (Renderer* renderer) {
    //program = renderer->getProgram("graphics_program").value();
    program = renderer->getProgram("default").value(); // TODO
    textProgram = renderer->getProgram("text").value();
    renderLayers.push_back(new MapRenderLayer(renderer));
    //renderLayers.push_back(new EntityRenderLayer());
    ids = renderer->getBufferIds(2, 40 * sizeof(float), {2, 2});
    buffers[0] = Buffer(40, sizeof(float), true);
    buffers[1] = Buffer(40, sizeof(float), true);
}


std::string GraphicsRenderLayer::getName() {
    return "graphics_layer";
}

int GraphicsRenderLayer::getPriority() {
    return 1;
}

bool GraphicsRenderLayer::isActive() {
    return active;
}

void GraphicsRenderLayer::gatherData () {
    for (auto i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->gatherData();
    }
}

void GraphicsRenderLayer::preRender(Renderer *renderer) {
    for (auto i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->preRender(renderer);
    }
}

void GraphicsRenderLayer::render(Renderer *renderer, FrameBuffer *frameBuf) {
    for (auto i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->render(renderer, frameBuf); // TODO
    }
    textProgram->useProgram();

    /*float posData[] = {0.0f, 0.0f, 0.0f, (float)80 / 600 * scale, (float)80 / 800 * scale, 0.0f, (float)80 / 800 * scale, 0.0f,
                       (float)80 / 800 * scale, (float)80 / 600 * scale, 0.0f, (float)80 / 600 * scale};*/
    float posData[] = {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
                       1.0f, 1.0f, -1.0f, 1.0f};
    float uvData[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                      1.0f, 0.0f, 0.0f, 0.0f};
    //float posData[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    buffers[0].pushData(posData, 12);
    buffers[1].pushData(uvData, 12);
    tex.bindTexture();
    renderer->bufferData(ids, buffers);
    renderer->render(ids, textProgram, 2);
    scale = 0.8f;
}

void GraphicsRenderLayer::applyCamera(Camera camera) {
    for (auto i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->applyCamera(camera);
    }
}

int GraphicsRenderLayer::getUniformId(std::string uniformName) {
    return 0; // TODO
}

void GraphicsRenderLayer::applyUniform(int uniformId, void *data) {
    // TODO
}

std::optional<RenderLayer*> GraphicsRenderLayer::getRenderLayer (const std::string& layer) {
    for (auto i : renderLayers) {
        if (i->getName() == layer) {
            // TODO: optimize
            return std::optional(i);
        }
    }
    return std::nullopt;
}

void GraphicsRenderLayer::addRenderLayer (RenderLayer* layer) {
    renderLayers.push_back(layer);
}
