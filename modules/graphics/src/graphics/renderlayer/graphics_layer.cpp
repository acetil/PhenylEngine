#include "graphics/renderlayer/graphics_layer.h"
#include "graphics/renderlayer/map_layer.h"
#include "entity_layer.h"

#include "graphics/font/font_manager.h"

using namespace graphics;

GraphicsRenderLayer::GraphicsRenderLayer (Renderer* renderer) {
    //program = renderer->getProgram("graphics_program").value();
    program = renderer->getProgramNew("default").orThrow(); // TODO
    //renderLayers.push_back(std::make_shared<MapRenderLayer>(renderer));
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
    for (const auto& i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->gatherData();
    }
}

void GraphicsRenderLayer::preRender(Renderer *renderer) {
    for (const auto& i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->preRender(renderer);
    }
}

void GraphicsRenderLayer::render(Renderer *renderer, FrameBuffer *frameBuf) {
    for (const auto& i : renderLayers) {
        if (!i->isActive()) {
            continue;
        }
        i->render(renderer, frameBuf); // TODO
    }

}

void GraphicsRenderLayer::applyCamera(Camera camera) {
    for (const auto& i : renderLayers) {
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

std::optional<std::shared_ptr<RenderLayer>> GraphicsRenderLayer::getRenderLayer (const std::string& layer) {
    for (const auto& i : renderLayers) {
        if (i->getName() == layer) {
            // TODO: optimize
            return std::optional(i);
        }
    }
    return std::nullopt;
}

void GraphicsRenderLayer::addRenderLayer (std::shared_ptr<RenderLayer> layer) {
    renderLayers.push_back(std::move(layer));
    std::sort(renderLayers.begin(), renderLayers.end(), [] (const std::shared_ptr<RenderLayer>& a, const std::shared_ptr<RenderLayer>& b) {
        return a->getPriority() < b->getPriority();
    });
}
