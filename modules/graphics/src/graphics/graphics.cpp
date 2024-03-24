#include "graphics/graphics.h"

#include <chrono>
#include <thread>
#include <unordered_set>
#include <utility>
#include "graphics/renderlayer/entity_layer.h"
#include "logging/logging.h"
#include "graphics/detail/loggers.h"
#include "graphics/renderlayer/graphics_layer.h"
#include "graphics/font/font_manager.h"
#include "graphics/phenyl_graphics.h"
#include "common/input/proxy_source.h"
#include "graphics/renderlayer/debug_layer.h"
#include "component/component_serializer.h"
#include "graphics/components/2d/sprite.h"
#include "graphics/components/2d/sprite_serialization.h"
#include "graphics/renderlayer/particle_layer.h"
#include "graphics/components/2d/particle_emitter.h"
#include "graphics/components/2d/particle_emitter_serialization.h"

using namespace phenyl::graphics;

void detail::Graphics::addEntityLayer (component::EntityComponentManager* compManager) {
    renderLayer->addRenderLayer(std::make_shared<EntityRenderLayer>(renderer.get(), compManager));
}


detail::Graphics::Graphics (std::unique_ptr<Renderer> renderer){
    this->renderer = std::move(renderer);
    this->deltaTime = 0;
    this->lastTime = this->renderer->getCurrentTime();

    this->renderer->loadDefaultShaders();
    this->renderLayer = std::make_shared<GraphicsRenderLayer>(this->renderer.get());
    renderLayer->addRenderLayer(makeDebugLayer(this->renderer.get()));
}

double detail::Graphics::getDeltaTime() const {
    return deltaTime;
}

void detail::Graphics::render () {
    renderer->clearWindow();
    if (!renderLayer->isActive()) {
        return;
    }
    FrameBuffer* buf = renderer->getWindowBuffer();


    renderLayer->gatherData();

    renderLayer->preRender(renderer.get());

    renderLayer->applyCamera(camera);

    renderLayer->render(renderer.get(), buf);

    renderer->finishRender();

}

void detail::Graphics::sync (int fps) {
    while (renderer->getCurrentTime() - lastTime < 1.0 / (fps)) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }
    double currTime = renderer->getCurrentTime();
    deltaTime = currTime - lastTime;
    lastTime = currTime;
}

[[maybe_unused]] Camera & detail::Graphics::getCamera () {
    return camera;
}

std::shared_ptr<GraphicsRenderLayer> detail::Graphics::getRenderLayer () {
    return renderLayer;
}


std::vector<std::shared_ptr<phenyl::common::InputSource>> detail::Graphics::getInputSources () {
    return renderer->getViewport().getInputSources();
}

detail::Graphics::~Graphics () {

}

std::string_view detail::Graphics::getName () const noexcept {
    return "Graphics";
}

std::vector<std::shared_ptr<phenyl::common::ProxySource>> detail::Graphics::getProxySources () {
    return renderer->getViewport().getProxySources();
}