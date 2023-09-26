#include <exception>
#include <fstream>

#include "component/component.h"

#include "engine/engine.h"
#include "component/component_serializer.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"
#include "graphics/ui/debug_ui.h"

#include "physics/physics.h"

#include "logging/logging.h"
#include "common/components/2d/global_transform.h"
#include "common/components/2d/global_transform_serialize.h"
#include "physics/physics.h"
#include "component/prefab_manager.h"
#include "engine/level/level_manager.h"
#include "util/profiler.h"

#define FIXED_FPS 60.0

using namespace phenyl;

class engine::detail::Engine {
private:
    //game::PhenylGameHolder gameObjHolder;
    graphics::PhenylGraphicsHolder graphicsHolder;
    component::EntityComponentManager componentManager;
    std::unique_ptr<component::EntitySerializer> entitySerializer;
    std::unique_ptr<physics::IPhysics> physicsObj;
    std::unique_ptr<component::PrefabManager> prefabManager;
    std::unique_ptr<game::LevelManager> levelManager;
    game::GameCamera gameCamera;
    game::GameInput gameInput;

    bool doDebugRender = false;
    bool doProfileRender = true;

    void setupCallbacks ();
    void addDefaultSerialisers ();
    void addComponents ();
public:
    Engine ();
    ~Engine();

    //[[nodiscard]] game::detail::GameObject::SharedPtr getGameObjectTemp () const;
    //[[nodiscard]] game::PhenylGame getGameObject () const;
    [[nodiscard]] graphics::PhenylGraphics getGraphics () const;
    component::EntityComponentManager& getComponentManager ();
    [[nodiscard]] const component::EntityComponentManager& getComponentManager () const;
    component::EntitySerializer& getEntitySerializer ();
    physics::PhenylPhysics getPhysics ();

    game::GameCamera& getCamera ();
    game::GameInput& getInput ();

    void dumpLevel (std::ostream& file);

    void updateEntityPosition (float deltaTime);
    void debugRender ();

    void gameloop (Application* app);
    void update (Application* app, double deltaTime);
    void fixedUpdate (Application* app);
    void render (Application* app, double deltaTime);

    void setDebugRender (bool doRender);
    void setProfileRender (bool doRender);
};

engine::PhenylEngine::PhenylEngine () {
    //internal = std::make_unique<engine::detail::Engine>();
    logger::initLogger();
}

engine::PhenylEngine::~PhenylEngine () = default;

graphics::PhenylGraphics engine::PhenylEngine::getGraphics () {
    return internal->getGraphics();
}

component::EntityComponentManager& engine::PhenylEngine::getComponentManager () {
    return internal->getComponentManager();
}

component::EntitySerializer& engine::PhenylEngine::getEntitySerializer () {
    return internal->getEntitySerializer();
}

physics::PhenylPhysics engine::PhenylEngine::getPhysics () {
    return internal->getPhysics();
}

void engine::PhenylEngine::updateEntityPosition (float deltaTime) {
    internal->updateEntityPosition(deltaTime);
}

void engine::PhenylEngine::debugRender () {
    internal->debugRender();
}

void engine::PhenylEngine::dumpLevel (std::ostream& file) {
    internal->dumpLevel(file);
}

game::GameCamera& engine::PhenylEngine::getCamera () {
    return internal->getCamera();
}

game::GameInput& engine::PhenylEngine::getInput () {
    return internal->getInput();
}

void engine::PhenylEngine::exec (Application* app) {
    internal = std::make_unique<engine::detail::Engine>();
    app->init();
    internal->gameloop(app);
    app->shutdown();
    internal = nullptr;
}

void engine::PhenylEngine::setDebugRender (bool doRender) {
    internal->setDebugRender(doRender);
}

void engine::PhenylEngine::setProfileRender (bool doRender) {
    internal->setProfileRender(doRender);
}

engine::detail::Engine::Engine () : componentManager{256} {
    entitySerializer = std::make_unique<component::EntitySerializer>();
    physicsObj = physics::makeDefaultPhysics();

    prefabManager = std::make_unique<component::PrefabManager>(&componentManager, entitySerializer.get());
    levelManager = std::make_unique<game::LevelManager>(&componentManager, entitySerializer.get());
    prefabManager->selfRegister();
    levelManager->selfRegister();
    setupCallbacks();

    //auto gameObj = gameObjHolder.getGameObject();
    auto graphics = graphicsHolder.getGraphics();
    gameInput.setRenderer(graphics.getRenderer());

    //gameObj.setEntityComponentManager(&componentManager);
    //gameObj.setSerializer(entitySerializer.get());

    addComponents();

    addDefaultSerialisers();
    //gameObj.addDefaultSerialisers();
    graphics.addComponentSerializers(getEntitySerializer());
    physicsObj->addComponentSerializers(getEntitySerializer());
    //physics::addComponentSerialisers(getEntitySerialiser());

    // TODO: move all to user
    //graphics.getTextureAtlas("sprite").ifPresent([&gameObj](auto& atlas){gameObj.setTextureIds(atlas);});
    graphics.addEntityLayer(&componentManager); // TODO: unhackify
    graphics.getUIManager().addRenderLayer(graphics.tempGetGraphics(), graphics.getRenderer());

    //gameObjHolder.initGame(graphics, eventBus);

    //gameObjHolder.getGameObject().getGameInput().addInputSources(graphicsHolder.tempGetGraphics()->getInputSources());
    gameInput.addInputSources(graphicsHolder.tempGetGraphics()->getInputSources());
}

engine::detail::Engine::~Engine () {
    logger::log(LEVEL_INFO, "MAIN", "Shutting down!");
    prefabManager->clear();
    componentManager.clearAll();
}

graphics::PhenylGraphics engine::detail::Engine::getGraphics () const {
    return graphicsHolder.getGraphics();
}

void engine::detail::Engine::setupCallbacks () {
    graphicsHolder.tempGetGraphics()->setupWindowCallbacks();
}

component::EntityComponentManager& engine::detail::Engine::getComponentManager () {
    return componentManager;
}

const component::EntityComponentManager& engine::detail::Engine::getComponentManager () const {
    return componentManager;
}

component::EntitySerializer& engine::detail::Engine::getEntitySerializer () {
    return *entitySerializer;
}

void engine::detail::Engine::addDefaultSerialisers () {
    //entitySerialiser->addComponentSerialiser<component::Position2D>("Position2D");
    entitySerializer->addSerializer<common::GlobalTransform2D>();
}

physics::PhenylPhysics engine::detail::Engine::getPhysics () {
    return physics::PhenylPhysics(physicsObj.get());
}

game::GameCamera& engine::detail::Engine::getCamera () {
    return gameCamera;
}

game::GameInput& engine::detail::Engine::getInput () {
    return gameInput;
}

void engine::detail::Engine::updateEntityPosition (float deltaTime) {
    // TODO: remove function?
    physicsObj->updatePhysics(getComponentManager(), deltaTime);
    physicsObj->checkCollisions(getComponentManager(), deltaTime);
}

void engine::detail::Engine::debugRender () {
    if (!doDebugRender) {
        return;
    }
    physicsObj->debugRender(getComponentManager());
}

void engine::detail::Engine::addComponents () {
    componentManager.addComponent<common::GlobalTransform2D>();
    physicsObj->addComponents(componentManager);
    graphicsHolder.tempGetGraphics()->addComponents(componentManager);
}

void engine::detail::Engine::dumpLevel (std::ostream& file) {
    levelManager->dump(file);
}

void engine::detail::Engine::gameloop (Application* app) {
    double deltaPhysicsFrame = 0.0f;
    auto graphics = graphicsHolder.getGraphics();
    logger::log(LEVEL_DEBUG, "ENGINE", "Starting loop!");
    while (!graphics.shouldClose()) {
        util::startProfileFrame();

        graphics.updateUI();
        gameInput.poll();

        double deltaTime = graphics.getDeltaTime();
        deltaPhysicsFrame += deltaTime * app->fixedTimeScale;

        util::startProfile("physics");
        while (deltaPhysicsFrame >= 1.0 / FIXED_FPS) {
            fixedUpdate(app);
            deltaPhysicsFrame -= 1.0 / FIXED_FPS;
        }
        util::endProfile();

        util::startProfile("graphics");
        update(app, deltaTime);
        render(app, deltaTime);
        util::endProfile();

        util::endProfileFrame();

        graphics.sync((int)app->targetFps); // TODO
        graphics.pollEvents();
    }
}

void engine::detail::Engine::update (Application* app, double deltaTime) {
    app->update(deltaTime);
}

void engine::detail::Engine::fixedUpdate (Application* app) {
    app->fixedUpdate(1.0 / FIXED_FPS);
    updateEntityPosition(1.0f / FIXED_FPS);
    getCamera().updateCamera(getGraphics().getCamera());
}

void engine::detail::Engine::render (Application* app, double deltaTime) {
    if (doProfileRender) {
        graphics::renderDebugUi(graphicsHolder.getGraphics().getUIManager(), (float) deltaTime);
    }
    debugRender();
    getGraphics().getUIManager().renderUI();

    getGraphics().render();
}

void engine::detail::Engine::setDebugRender (bool doRender) {
    doDebugRender = doRender;
}

void engine::detail::Engine::setProfileRender (bool doRender) {
    doProfileRender = doRender;
}
