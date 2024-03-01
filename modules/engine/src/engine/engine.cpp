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
#include "common/components/timed_lifetime.h"
#include "common/components/timed_lifetime_serialize.h"
#include "physics/physics.h"
#include "component/prefab_manager.h"
#include "engine/level/level_manager.h"
#include "util/profiler.h"

#define FIXED_FPS 60.0

using namespace phenyl;

static Logger LOGGER{"ENGINE"};

class engine::detail::Engine {
private:
    //game::PhenylGameHolder gameObjHolder;
    graphics::PhenylGraphicsHolder graphicsHolder;
    component::EntityComponentManager componentManager;
    std::unique_ptr<component::EntitySerializer> entitySerializer;
    std::unique_ptr<physics::IPhysics> physicsObj;
    std::unique_ptr<component::PrefabManager> prefabManager;
    std::unique_ptr<game::LevelManager> levelManager;
    std::unique_ptr<audio::AudioSystem> audioSystem;
    game::GameCamera gameCamera;
    game::GameInput gameInput;

    bool doDebugRender = false;
    bool doProfileRender = true;

    void setupCallbacks ();
    void addDefaultSerialisers ();
    void addComponents ();
public:
    Engine (const ApplicationProperties& properties);
    ~Engine();

    //[[nodiscard]] game::detail::GameObject::SharedPtr getGameObjectTemp () const;
    //[[nodiscard]] game::PhenylGame getGameObject () const;
    [[nodiscard]] graphics::PhenylGraphics getGraphics () const;
    component::EntityComponentManager& getComponentManager ();
    [[nodiscard]] const component::EntityComponentManager& getComponentManager () const;
    component::EntitySerializer& getEntitySerializer ();
    physics::PhenylPhysics getPhysics ();
    audio::AudioSystem& getAudio ();

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
    InitLogging(PHENYL_DEFAULT_ROOT_LEVEL, "debug.log");
}

engine::PhenylEngine::~PhenylEngine () {
    ShutdownLogging();
}

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
    internal = std::make_unique<engine::detail::Engine>(app->properties);
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

audio::AudioSystem& phenyl::engine::PhenylEngine::getAudio () {
    return internal->getAudio();
}

engine::detail::Engine::Engine (const ApplicationProperties& properties) : graphicsHolder(properties.graphicsProperties), componentManager{256} {
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

    audioSystem = audio::MakeOpenALSystem();
    audioSystem->selfRegister();

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
    PHENYL_LOGI(LOGGER, "Shutting down!");
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
    entitySerializer->addSerializer<common::TimedLifetime>();
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
    componentManager.addComponent<common::TimedLifetime>();
    physicsObj->addComponents(componentManager);
    graphicsHolder.tempGetGraphics()->addComponents(componentManager);
    audioSystem->addComponents(componentManager,  *entitySerializer);
}

void engine::detail::Engine::dumpLevel (std::ostream& file) {
    levelManager->dump(file);
}

void engine::detail::Engine::gameloop (Application* app) {
    double deltaPhysicsFrame = 0.0f;
    auto graphics = graphicsHolder.getGraphics();
    PHENYL_LOGD(LOGGER, "Starting loop!");
    while (!graphics.shouldClose()) {
        PHENYL_LOGT(LOGGER, "Frame start");
        util::startProfileFrame();

        graphics.updateUI();
        gameInput.poll();

        double deltaTime = graphics.getDeltaTime();
        deltaPhysicsFrame += deltaTime * app->fixedTimeScale;

        util::startProfile("physics");
        while (deltaPhysicsFrame >= 1.0 / FIXED_FPS) {
            PHENYL_LOGT(LOGGER, "Physics frame start");
            fixedUpdate(app);
            deltaPhysicsFrame -= 1.0 / FIXED_FPS;
            PHENYL_LOGT(LOGGER, "Physics frame end");
        }
        util::endProfile();

        util::startProfile("graphics");
        update(app, deltaTime);
        render(app, deltaTime);
        util::endProfile();

        util::endProfileFrame();

        graphics.sync((int)app->targetFps); // TODO
        graphics.pollEvents();
        PHENYL_LOGT(LOGGER, "Frame end");
    }
}

void engine::detail::Engine::update (Application* app, double deltaTime) {
    PHENYL_LOGT(LOGGER, "Update start");
    app->update(deltaTime);
    audioSystem->update((float)deltaTime);
    graphicsHolder.getGraphics().frameUpdate(componentManager);
    common::TimedLifetime::Update(componentManager, deltaTime); // TODO: put somewhere else
    PHENYL_LOGT(LOGGER, "Update end");
}

void engine::detail::Engine::fixedUpdate (Application* app) {
    PHENYL_LOGT(LOGGER, "Fixed update start");
    app->fixedUpdate(1.0 / FIXED_FPS);
    updateEntityPosition(1.0f / FIXED_FPS);
    getCamera().updateCamera(getGraphics().getCamera());
    PHENYL_LOGT(LOGGER, "Fixed update end");
}

void engine::detail::Engine::render (Application* app, double deltaTime) {
    PHENYL_LOGT(LOGGER, "Render start");
    if (doProfileRender) {
        graphics::renderDebugUi(graphicsHolder.getGraphics().getUIManager(), (float) deltaTime);
    }
    debugRender();
    getGraphics().getUIManager().renderUI();

    getGraphics().render();
    PHENYL_LOGT(LOGGER, "Render end");
}

void engine::detail::Engine::setDebugRender (bool doRender) {
    doDebugRender = doRender;
}

void engine::detail::Engine::setProfileRender (bool doRender) {
    doProfileRender = doRender;
}

audio::AudioSystem& engine::detail::Engine::getAudio () {
    return *audioSystem;
}
