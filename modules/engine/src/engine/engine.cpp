#include <exception>

#include "component/component.h"

#include "engine/engine.h"
#include "engine/game_init.h"
#include "engine/entity/entities.h"
#include "engine/entity/controller/entity_controller.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"
#include "graphics/graphics_update.h"
#include "graphics/ui/debug_ui.h"

#include "physics/physics.h"

#include "logging/logging.h"

using namespace engine;

class engine::detail::Engine {
private:
    //graphics::detail::Graphics::SharedPtr graphics;
    event::EventBus::SharedPtr eventBus;
    game::PhenylGameHolder gameObjHolder;
    graphics::PhenylGraphicsHolder graphicsHolder;
    component::EntityComponentManager::SharedPtr componentManager;

    void addEventHandlers ();

public:
    Engine ();
    ~Engine();

    [[nodiscard]] game::detail::GameObject::SharedPtr getGameObjectTemp () const;
    [[nodiscard]] game::PhenylGame getGameObject () const;
    [[nodiscard]] graphics::PhenylGraphics getGraphics () const;
    component::EntityComponentManager::SharedPtr getComponentManager () const;
    event::EventBus::SharedPtr getEventBus ();
};

engine::PhenylEngine::PhenylEngine () {
    internal = std::make_unique<engine::detail::Engine>();
}

engine::PhenylEngine::~PhenylEngine () = default;

game::detail::GameObject::SharedPtr engine::PhenylEngine::getGameTemp () {
    return internal->getGameObjectTemp();
}

graphics::PhenylGraphics PhenylEngine::getGraphics () {
    return internal->getGraphics();
}

game::PhenylGame PhenylEngine::getGame () {
    return internal->getGameObject();
}

event::EventBus::SharedPtr PhenylEngine::getEventBus () {
    return internal->getEventBus();
}

component::EntityComponentManager::SharedPtr PhenylEngine::getComponentManager () {
    return internal->getComponentManager();
}

engine::detail::Engine::Engine () {
    /*GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        throw std::runtime_error("Window creation failed!");
    }

    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        throw std::runtime_error("Graphics init failed!");
    }

    logger::log(LEVEL_INFO, "MAIN", "Successfully initialised graphics");*/

    eventBus = event::EventBus::NewSharedPtr();
    componentManager = component::EntityComponentManager::NewSharedPtr(255);
    addEventHandlers();

    auto gameObj = gameObjHolder.getGameObject();
    auto graphics = graphicsHolder.getGraphics();

    gameObj.setEntityComponentManager(componentManager);

    logger::log(LEVEL_INFO, "GAME", "Starting init of entities!");
    eventBus->raiseEvent(event::EntityRegisterEvent(gameObj));
    logger::log(LEVEL_DEBUG, "GAME", "Finished entity init!");

    // TODO: move all to user
    graphics.getTextureAtlas("sprite").ifPresent([&gameObj](auto& atlas){gameObj.setTextureIds(atlas);});
    graphics::addMapRenderLayer(graphics, eventBus);
    graphics.addEntityLayer(componentManager); // TODO: unhackify
    graphics.getUIManager().addRenderLayer(graphics.tempGetGraphics(), graphics.getRenderer());

    gameObjHolder.initGame(graphics, eventBus);

    gameObjHolder.getGameObject().getGameInput().addInputSources(graphicsHolder.tempGetGraphics()->getInputSources());
}

engine::detail::Engine::~Engine () {
    logger::log(LEVEL_INFO, "MAIN", "Shutting down!");
    //graphics::destroyGraphics(graphics);
}

game::detail::GameObject::SharedPtr detail::Engine::getGameObjectTemp () const {
    return gameObjHolder.tempGetGameObject();
}

graphics::PhenylGraphics detail::Engine::getGraphics () const {
    return graphicsHolder.getGraphics();
}

game::PhenylGame detail::Engine::getGameObject () const {
    return gameObjHolder.getGameObject();
}

event::EventBus::SharedPtr detail::Engine::getEventBus () {
    return eventBus;
}

void detail::Engine::addEventHandlers () {
    graphicsHolder.getGraphics().addEventHandlers(eventBus);
    gameObjHolder.getGameObject().addEventHandlers(eventBus);

    eventBus->subscribeHandler(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    eventBus->subscribeHandler(physics::onEntityCreation);
    //gameObject.getEventBus()->subscribeHandler(&graphics::detail::Graphics::onEntityCreation, graphics.tempGetGraphics());
    eventBus->subscribeHandler(graphics::updateEntityRotation);
    eventBus->subscribeHandler(physics::updateEntityHitboxRotation);

    graphics::addDebugEventHandlers(eventBus);

    game::addControlEventHandlers(eventBus);
}

component::EntityComponentManager::SharedPtr detail::Engine::getComponentManager () const {
    return componentManager;
}
