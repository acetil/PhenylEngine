#include "graphics_new.h"

#include <chrono>
#include <thread>
#include <unordered_set>
#include <graphics/renderlayer/entity_layer.h>
#include "logging/logging.h"
#include "game/entity/entity.h"
using namespace graphics;

GraphicsNew::GraphicsNew (Renderer* renderer) {
    this->renderer = renderer;

    this->deltaTime = 0;
    this->lastTime = renderer->getCurrentTime();
    this->renderLayer = new GraphicsRenderLayer(renderer);
}

double GraphicsNew::getDeltaTime() const {
    return deltaTime;
}

bool GraphicsNew::shouldClose() {
    return renderer->shouldClose();
}

void GraphicsNew::pollEvents() {
    renderer->pollEvents();
}

void GraphicsNew::render () {
    renderer->clearWindow();
    if (!renderLayer->isActive()) {
        return;
    }
    atlases["sprite"].bindTextureAtlas(); // TODO: get layers to specify
    FrameBuffer* buf = renderer->getWindowBuffer();

    BufferInfo info = std::move(renderLayer->getBufferInfo());



    if (!info.isEmpty()) {
        for (int i = 0; i < info.numBuffers; i++) {
            renderLayer->addBuffer(BufferNew(info.elementSizes[i].second * info.elementSizes[i].first * info.sizes[i],
                    info.elementSizes[i].first, info.isStatic[i]));
        }
    }

    renderLayer->gatherData();

    renderLayer->preRender(renderer);

    renderLayer->applyCamera(camera);

    renderLayer->render(renderer, buf);

    renderer->finishRender();

}

std::optional<TextureAtlas> GraphicsNew::getTextureAtlas (const std::string& atlas) {
    if (atlases.find(atlas) == atlases.end()) {
        logging::logf(LEVEL_ERROR, "Attempted to get nonexistent atlas \"%s\"", atlas.c_str());
        return std::nullopt;
    }
    return atlases[atlas];
}

void GraphicsNew::initTextureAtlas (const std::string& atlasName, const std::vector<Model>& images) {
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
    atlas.createAtlas(std::vector<Image*>(imageSet.begin(), imageSet.end()));
    atlas.loadTextureAtlas();
    logging::logf(LEVEL_DEBUG, "Created atlas \"%s\"!", atlasName.c_str());
    atlases[atlasName] = atlas;
}

void GraphicsNew::sync (int fps) {
    while (renderer->getCurrentTime() - lastTime < 1.0 / (2.0 * fps)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    double currTime = renderer->getCurrentTime();
    deltaTime = currTime - lastTime;
    lastTime = currTime;
}

Camera &GraphicsNew::getCamera () {
    return camera;
}

GraphicsRenderLayer* GraphicsNew::getRenderLayer () {
    return renderLayer;
}

void GraphicsNew::addEntityLayer (component::ComponentManager<game::AbstractEntity*>* compManager) {
    renderLayer->addRenderLayer(new EntityRenderLayer(renderer, compManager));
}

void GraphicsNew::onEntityCreation (event::EntityCreationEvent* event) {
    int texId = event->entity->getTextureId(); // TODO: update for models and decoupling
    unsigned int id = event->entity->getEntityId();
    auto* pointer = event->compManager->getObjectDataPtr<float>(2, id);
    TextureAtlas atlas = this->getTextureAtlas("sprite").value();
    Texture* tex = atlas.getTexture(texId);
    memcpy(pointer, tex->getTexUvs(), 12 * sizeof(float));
}


BufferNew::BufferNew (int maxNumElements, int elementSize, bool isStatic) {
    static_assert(sizeof(char) == 1, "Char has a greater than 1 size on this system!");
    memory = (void*) new char[maxNumElements * elementSize];
    this->maxNumElements = maxNumElements;
    this->elementSize = elementSize;
    this->isStatic = isStatic;
    this->numElements = 0;
    ownsMemory = true;
}

BufferNew::~BufferNew () {
    if (ownsMemory) {
        delete[] (char*)memory;
    }
}
BufferNew::BufferNew (const BufferNew& copy) {
    this->maxNumElements = copy.maxNumElements;
    this->elementSize = copy.elementSize;
    this->isStatic = copy.isStatic;
    this->numElements = copy.numElements;
    this->memory = copy.memory;
    ownsMemory = false; // copy constructor does not move memory ownership
}
BufferNew::BufferNew(BufferNew &&move)  noexcept {
    this->maxNumElements = move.maxNumElements;
    this->elementSize = move.elementSize;
    this->isStatic = move.isStatic;
    this->numElements = move.numElements;
    this->memory = move.memory;
    ownsMemory = true;
    move.ownsMemory = false; // moves memory ownership
}

BufferNew& BufferNew::operator=(BufferNew &&move) noexcept {
    if (this != &move) {
        if (ownsMemory) {
            delete[] (char*)memory;
        }
        memory = move.memory;
        this->maxNumElements = move.maxNumElements;
        this->elementSize = move.elementSize;
        this->isStatic = move.isStatic;
        this->numElements = move.numElements;
        ownsMemory = move.ownsMemory;
        move.ownsMemory = false;
    }
    return *this;
}
