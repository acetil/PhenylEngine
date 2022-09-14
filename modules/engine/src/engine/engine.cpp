#include <exception>

#include "component/component.h"

#include "engine/engine.h"
#include "engine/game_init.h"
#include "engine/entity/entities.h"
#include "engine/entity/controller/entity_controller.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"
#include "graphics/ui/debug_ui.h"

#include "physics/physics.h"

#include "logging/logging.h"
#include "component/component_serialiser.h"
#include "component/rotation_component.h"
#include "component/position.h"

using namespace engine;

class engine::detail::Engine {
private:
    //graphics::detail::Graphics::SharedPtr graphics;
    event::EventBus::SharedPtr eventBus;
    game::PhenylGameHolder gameObjHolder;
    graphics::PhenylGraphicsHolder graphicsHolder;
    component::EntityComponentManager::SharedPtr componentManager;
    std::unique_ptr<component::EntitySerialiser> entitySerialiser;
    void addEventHandlers ();
    void addDefaultSerialisers ();
public:
    Engine ();
    ~Engine();

    [[nodiscard]] game::detail::GameObject::SharedPtr getGameObjectTemp () const;
    [[nodiscard]] game::PhenylGame getGameObject () const;
    [[nodiscard]] graphics::PhenylGraphics getGraphics () const;
    component::EntityComponentManager::SharedPtr getComponentManager () const;
    event::EventBus::SharedPtr getEventBus ();
    component::EntitySerialiser& getEntitySerialiser ();
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

component::EntitySerialiser& PhenylEngine::getEntitySerialiser () {
    return internal->getEntitySerialiser();
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
    entitySerialiser = std::make_unique<component::EntitySerialiser>();
    addEventHandlers();

    auto gameObj = gameObjHolder.getGameObject();
    auto graphics = graphicsHolder.getGraphics();

    gameObj.setEntityComponentManager(componentManager);
    gameObj.setSerialiser(entitySerialiser.get());

    addDefaultSerialisers();
    gameObj.addDefaultSerialisers();
    graphics.addComponentSerialisers(getEntitySerialiser());
    physics::addComponentSerialisers(getEntitySerialiser());

    // TODO: move all to user
    //graphics.getTextureAtlas("sprite").ifPresent([&gameObj](auto& atlas){gameObj.setTextureIds(atlas);});
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

    eventBus->subscribeUnscoped(game::addEntities);
    //gameObject->getEventBus()->subscribeHandler(graphics::onEntityCreation);
    eventBus->subscribeUnscoped(physics::onEntityCreation);
    //gameObject.getEventBus()->subscribeHandler(&graphics::detail::Graphics::onEntityCreation, graphics.tempGetGraphics());
    //eventBus->subscribeUnscoped(graphics::updateEntityRotation);
    //eventBus->subscribeUnscoped(physics::updateEntityHitboxRotation);

    graphics::addDebugEventHandlers(eventBus);

    game::addControlEventHandlers(eventBus);
}

component::EntityComponentManager::SharedPtr detail::Engine::getComponentManager () const {
    return componentManager;
}

component::EntitySerialiser& detail::Engine::getEntitySerialiser () {
    return *entitySerialiser;
}

void detail::Engine::addDefaultSerialisers () {
    entitySerialiser->addComponentSerialiser<component::Rotation2D>("rotation_2D", [](const component::Rotation2D& comp) -> util::DataValue {
        return comp._serialise();
    }, [] (const util::DataValue& val) -> util::Optional<component::Rotation2D> {
        component::Rotation2D comp{};
        comp._deserialise(val);

        return {comp};
    });

    entitySerialiser->addComponentSerialiser<component::Position2D>("pos_2D", component::serialisePos2D, component::deserialisePos2D);
}
