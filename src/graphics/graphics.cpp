#include "graphics.h"

#include <chrono>
#include <thread>
#include <unordered_set>
#include <graphics/renderlayer/entity_layer.h>
#include "logging/logging.h"
#include "game/entity/entity.h"
#include "renderlayer/graphics_layer.h"
using namespace graphics;

Graphics::Graphics (Renderer* renderer) {
    this->renderer = renderer;

    this->deltaTime = 0;
    this->lastTime = renderer->getCurrentTime();
    this->renderLayer = new GraphicsRenderLayer(renderer);
}

double Graphics::getDeltaTime() const {
    return deltaTime;
}

bool Graphics::shouldClose() {
    return renderer->shouldClose();
}

void Graphics::pollEvents() {
    renderer->pollEvents();
}

void Graphics::render () {
    renderer->clearWindow();
    if (!renderLayer->isActive()) {
        return;
    }
    atlases["sprite"].bindTextureAtlas(); // TODO: get layers to specify
    FrameBuffer* buf = renderer->getWindowBuffer();




    renderLayer->gatherData();

    renderLayer->preRender(renderer);

    renderLayer->applyCamera(camera);

    renderLayer->render(renderer, buf);

    renderer->finishRender();

}

std::optional<TextureAtlas> Graphics::getTextureAtlas (const std::string& atlas) {
    if (atlases.find(atlas) == atlases.end()) {
        logging::logf(LEVEL_ERROR, "Attempted to get nonexistent atlas \"%s\"", atlas.c_str());
        return std::nullopt;
    }
    return atlases[atlas];
}

void Graphics::initTextureAtlas (const std::string& atlasName, const std::vector<Model>& images) {
    // TODO: build baked models
    if (atlases.find(atlasName) != atlases.end()) {
        logging::logf(LEVEL_ERROR, "Attempted to create duplicate atlas \"%s\"!", atlasName.c_str());
        return;
    }
    std::unordered_set<Image*> imageSet;
    for (const auto& m : images) {
        for (auto p : m.textures) {
            imageSet.insert(p.second);
        }
    }
    TextureAtlas atlas;
    atlas.createAtlas(images);
    atlas.loadTextureAtlas(renderer);
    logging::logf(LEVEL_DEBUG, "Created atlas \"%s\"!", atlasName.c_str());
    atlases[atlasName] = atlas;
}

void Graphics::sync (int fps) {
    while (renderer->getCurrentTime() - lastTime < 1.0 / (2.0 * fps)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    double currTime = renderer->getCurrentTime();
    deltaTime = currTime - lastTime;
    lastTime = currTime;
}

[[maybe_unused]] Camera &Graphics::getCamera () {
    return camera;
}

GraphicsRenderLayer* Graphics::getRenderLayer () {
    return renderLayer;
}

void Graphics::addEntityLayer (component::EntityComponentManager* compManager) {
    renderLayer->addRenderLayer(new EntityRenderLayer(renderer, compManager));
}

void Graphics::onEntityCreation (event::EntityCreationEvent& event) {
    int texId = event.entity->getTextureId(); // TODO: update for models and decoupling
    unsigned int id = event.entity->getEntityId();
    auto data = event.compManager->getObjectDataPtr<graphics::FixedModel>(id);
    TextureAtlas atlas = this->getTextureAtlas("sprite").value();
    *data = atlas.getModel(texId);
    //Texture* tex = atlas.getTexture(texId);
    //memcpy(pointer, tex->getTexUvs(), 12 * sizeof(float));
}