#include "graphics_layer.h"
#include "map_layer.h"
#include "entity_layer.h"

#include "graphics/font/font_manager.h"

using namespace graphics;

GraphicsRenderLayer::GraphicsRenderLayer (Renderer* renderer, FontManager& _manager) : manager(std::move(_manager)), font(manager.getFace("noto-serif"), 64) {
    //program = renderer->getProgram("graphics_program").value();
    program = renderer->getProgram("default").value(); // TODO
    textProgram = renderer->getProgram("text").value();
    renderLayers.push_back(new MapRenderLayer(renderer));
    //renderLayers.push_back(new EntityRenderLayer());
    ids = renderer->getBufferIds(2, 40 * 12 * sizeof(float), {2, 2});
    buffers[0] = Buffer(40 * 12, sizeof(float), true);
    buffers[1] = Buffer(40 * 12, sizeof(float), true);
    font.loadAtlas(renderer);
    // TODO: remove
    //graphics::FontManager manager;
    //manager.addFace("noto-serif", "/usr/share/fonts/noto/NotoSerif-Regular.ttf");
    //auto& face = manager.getFace("noto-serif");
    //face.setFontSize(72);
    //face.setGlyphs({graphics::AsciiGlyphRange});
    //graphics::GlyphAtlas atlas(face.getGlyphs(), 64);
    //atlas.loadAtlas(graphics->getRenderer());
    //graphics->addGlyphAtlas(atlas);
    //font(face, 64);
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
                       (float)80 / 800 * scale, (float)80 / 600 * scale, 0.0f, (float)80 / 600 * scale};
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
    scale = 0.8f;*/
    font.bindAtlasTexture();
    font.renderText("Hello World!", 36, 10, 300, buffers);
    int size = buffers->currentSize() / 3 / 4 / 2;
    //int size2 = buffers[1].currentSize() / 3 / 4 / 2;
    //logging::log(LEVEL_DEBUG, "Size: {}", size);
    //logging::log(LEVEL_DEBUG, "Size 2: {}", size2);
    renderer->bufferData(ids, buffers);
    renderer->render(ids, textProgram, size);
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
