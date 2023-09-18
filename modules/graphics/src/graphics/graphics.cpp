#include "graphics/graphics.h"

#include <chrono>
#include <thread>
#include <unordered_set>
#include <utility>
#include "graphics/renderlayer/entity_layer.h"
#include "logging/logging.h"
#include "graphics/renderlayer/graphics_layer.h"
#include "graphics/renderers/window_callbacks.h"
#include "graphics/font/font_manager.h"
#include "graphics/phenyl_graphics.h"
#include "common/input/proxy_source.h"
#include "graphics/renderlayer/debug_layer.h"
#include "common/events/debug/debug_pause.h"
#include "component/component_serializer.h"
#include "graphics/components/2d/sprite.h"
#include "graphics/components/2d/sprite_serialization.h"

using namespace graphics;

void detail::Graphics::addEntityLayer (component::EntityComponentManager* compManager) {
    renderLayer->addRenderLayer(std::make_shared<EntityRenderLayer>(renderer.get(), compManager));
}

void detail::Graphics::setupWindowCallbacks (const event::EventBus::SharedPtr& bus) {
    //auto ctx = std::make_unique<WindowCallbackContext>();
    //ctx->graphics = this;
    //ctx->eventBus = std::move(bus);
    //ctx->renderer = renderer;
    //renderer->setupWindowCallbacks(std::move(ctx));
    renderer->setupCallbacks(bus);
    logging::log(LEVEL_DEBUG, "Set up window callbacks!");
}

detail::Graphics::Graphics (std::unique_ptr<Renderer> renderer, FontManager& manager) : uiManager(renderer.get(), manager) {
    this->deltaTime = 0;
    this->lastTime = renderer->getCurrentTime();

    this->renderLayer = std::make_shared<GraphicsRenderLayer>(renderer.get());
    renderLayer->addRenderLayer(makeDebugLayer(renderer.get()));

    auto rendererSources = renderer->getInputSources();

    for (auto& i : rendererSources) {
        inputSources.emplace_back(std::make_shared<common::ProxySource>(i));
    }

    uiManager.addProxyInputSources(inputSources);
    uiManager.setupInputActions();
    this->renderer = std::move(renderer);
}

double detail::Graphics::getDeltaTime() const {
    return deltaTime;
}

bool detail::Graphics::shouldClose() {
    return renderer->shouldClose();
}

void detail::Graphics::pollEvents() {
    renderer->pollEvents();
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

void detail::Graphics::onEntityCreation (event::EntityCreationEvent& event) {
    /*int texId = event.entityView.get<std::shared_ptr<game::EntityController>>().getUnsafe()->getTextureId(event.entityView, event.gameView); // TODO: update for models and decoupling
    //TextureAtlas atlas = this->getTextureAtlas("sprite").value();
    getTextureAtlas("sprite").ifPresent([&event, &texId](auto& atlas) {

        event.compManager->insert<Model2D>(event.entityId, atlas.getModel(texId));

        //event.entityView.model = atlas.getModel((texId));
    });*/

    /*event.entityView.get<Model2D>().ifPresent([this] (Model2D& comp) {
        getTextureAtlas("sprite").ifPresent([&comp] (TextureAtlas& atlas) {
            comp = atlas.getModel(comp.modelName);
        });
    });*/
    /*event.entityView.apply<Model2D>([this] (auto& info, Model2D& comp) {
        getTextureAtlas("sprite").ifPresent([&comp] (TextureAtlas& atlas) {
            comp = atlas.getModel(comp.modelName);
        });
    });*/

    //Texture* tex = atlas.getTexture(texId);
    //memcpy(pointer, tex->getTexUvs(), 12 * sizeof(float));
}

UIManager& detail::Graphics::getUIManager () {
    return uiManager;
}

void detail::Graphics::deleteWindowCallbacks () {
    renderer->invalidateWindowCallbacks();
}

void detail::Graphics::addEventHandlers (const event::EventBus::SharedPtr& eventBus) {
    //eventBus->subscribeHandler(&graphics::detail::Graphics::onEntityCreation, );
    eventScope = eventBus->getScope();
    eventBus->subscribe(&graphics::detail::Graphics::onEntityCreation, this, eventScope);
    eventBus->subscribe(&graphics::detail::Graphics::onMousePosChange, this, eventScope);
    eventBus->subscribe(&graphics::detail::Graphics::onThemeChange, this, eventScope);

    eventBus->subscribe<event::DebugPauseEvent>([this] (event::DebugPauseEvent& event) {
        if (event.doPause) {
            this->timeIsPaused = true;
            pauseStartTime = renderer->getCurrentTime();
        } else if (this->timeIsPaused) {
            this->timeIsPaused = false;
            lastTime += renderer->getCurrentTime() - pauseStartTime;
        }
    }, eventScope);

    setupWindowCallbacks(eventBus);
}

void detail::Graphics::onMousePosChange (event::CursorPosChangeEvent& event) {
    uiManager.setMousePos(event.windowPos);
}

std::vector<std::shared_ptr<common::InputSource>> detail::Graphics::getInputSources () {
    std::vector<std::shared_ptr<common::InputSource>> proxies;
    for (auto& i : inputSources) {
        proxies.emplace_back(i->getProxy());
    }

    return proxies;
}

void detail::Graphics::onThemeChange (event::ChangeThemeEvent& event) {
    uiManager.setCurrentTheme(common::Assets::Load<graphics::ui::Theme>(event.themeName));
}

detail::Graphics::~Graphics () {

}

void detail::Graphics::updateUI () {
    uiManager.updateUI();
}

void detail::Graphics::addComponentSerializers (component::EntitySerializer& serialiser) {
    //serialiser.addSerializer<graphics::Model2D>();
    serialiser.addSerializer<graphics::Sprite2D>();
    //serialiser.addComponentSerialiser<graphics::GlobalTransform2D>("GlobalTransform2D");
}

void detail::Graphics::addComponents (component::ComponentManager& manager) {
    //manager.addComponent<graphics::Model2D>();
    manager.addComponent<graphics::Sprite2D>();
}
