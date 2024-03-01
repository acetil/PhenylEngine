#include <exception>
#include <utility>

#include "graphics/phenyl_graphics.h"
#include "graphics/graphics.h"
#include "graphics/graphics_init.h"

using namespace phenyl::graphics;

PhenylGraphicsHolder::PhenylGraphicsHolder (const GraphicsProperties& properties) {
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window, properties) != GRAPHICS_INIT_SUCCESS) {
        PHENYL_ABORT("Window init failure!");
    }

    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        PHENYL_ABORT("Graphics init failure!");
    }

    PHENYL_LOGI(detail::GRAPHICS_LOGGER, "Successfully initialised graphics");
}

PhenylGraphics PhenylGraphicsHolder::getGraphics () const {
    return PhenylGraphics(graphics);
}

PhenylGraphicsHolder::~PhenylGraphicsHolder () {
    graphics::destroyGraphics(graphics);
}

std::shared_ptr<detail::Graphics> PhenylGraphics::getGraphics () const {
    auto ptr = graphics.lock();

    PHENYL_DASSERT_MSG(ptr, "Graphics attempted to be accessed after it was deleted!");

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

void PhenylGraphics::addEntityLayer (component::EntityComponentManager* componentManager) {
    getGraphics()->addEntityLayer(componentManager);
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

void PhenylGraphics::updateUI () {
    getGraphics()->updateUI();
}

void PhenylGraphics::addComponentSerializers (component::EntitySerializer& serialiser) {
    getGraphics()->addComponentSerializers(serialiser);
}

void PhenylGraphics::frameUpdate (phenyl::component::ComponentManager& componentManager) {
    getGraphics()->frameUpdate(componentManager);
}





