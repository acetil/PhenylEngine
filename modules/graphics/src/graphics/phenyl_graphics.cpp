#include <exception>
#include <utility>

#include "graphics/phenyl_graphics.h"
#include "graphics/graphics.h"
#include "graphics/graphics_init.h"

using namespace graphics;

PhenylGraphicsHolder::PhenylGraphicsHolder () {
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        throw std::runtime_error("Window creation failed!");
    }

    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        throw std::runtime_error("Graphics init failed!");
    }

    logger::log(LEVEL_INFO, "MAIN", "Successfully initialised graphics");
}

PhenylGraphics PhenylGraphicsHolder::getGraphics () const {
    return PhenylGraphics(graphics);
}

PhenylGraphicsHolder::~PhenylGraphicsHolder () {
    graphics::destroyGraphics(graphics);
}

std::shared_ptr<detail::Graphics> PhenylGraphics::getGraphics () const {
    auto ptr = graphics.lock();

#ifndef NDEBUG
    if (!ptr) {
        logging::log(LEVEL_FATAL, "Graphics attempted to be accessed after it was deleted!");
        throw std::runtime_error("Graphics attempted to be accessed after it was deleted!");
    }
#endif
    return ptr;
}


bool PhenylGraphics::shouldClose () const {
    return getGraphics()->shouldClose();
}

void PhenylGraphics::pollEvents () {
    return getGraphics()->pollEvents();
}

void PhenylGraphics::render () {
    getGraphics()->render();
}

void PhenylGraphics::initTextureAtlas (const std::string& atlasName, const std::vector<Model>& images) {
    getGraphics()->initTextureAtlas(atlasName, images);
}

util::Optional<graphics::TextureAtlas&> PhenylGraphics::getTextureAtlas (const std::string& atlas) const {
    return getGraphics()->getTextureAtlas(atlas);
}

void PhenylGraphics::sync (int fps) {
    getGraphics()->sync(fps);
}

double PhenylGraphics::getDeltaTime () const {
    return getGraphics()->getDeltaTime();
}

std::shared_ptr<GraphicsRenderLayer> PhenylGraphics::getRenderLayer () const {
    return getGraphics()->getRenderLayer();
}

Camera& PhenylGraphics::getCamera () {
    return getGraphics()->getCamera();
}

void PhenylGraphics::addEntityLayer (component::EntityComponentManager::SharedPtr componentManager) {
    getGraphics()->addEntityLayer(std::move(componentManager));
}

void PhenylGraphics::onEntityCreation (event::EntityCreationEvent& event) {
    getGraphics()->onEntityCreation(event);
}

Renderer* PhenylGraphics::getRenderer () const {
    return getGraphics()->getRenderer();
}

/*void PhenylGraphics::setupWindowCallbacks (const event::EventBus::SharedPtr& bus) {
    getGraphics()->setupWindowCallbacks(bus);
}*/

void PhenylGraphics::deleteWindowCallbacks () {
    getGraphics()->deleteWindowCallbacks();
}

UIManager& PhenylGraphics::getUIManager () {
    return getGraphics()->getUIManager();
}

void PhenylGraphics::addEventHandlers (const event::EventBus::SharedPtr& eventBus) {
    getGraphics()->addEventHandlers(eventBus);
}

void PhenylGraphics::updateUI () {
    getGraphics()->updateUI();
}

void PhenylGraphics::addComponentSerialisers (component::EntitySerialiser& serialiser) {
    getGraphics()->addComponentSerialisers(serialiser);
}





