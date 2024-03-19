#include <exception>
#include <fstream>

#include "component/component.h"

#include "engine/engine.h"
#include "engine/detail/loggers.h"
#include "component/component_serializer.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"
#include "graphics/ui/debug_ui.h"
#include "graphics/ui/ui_manager.h"

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

#include "runtime/runtime.h"
#include "common/debug.h"
#include "audio/audio_plugin.h"

#define FIXED_FPS 60.0

using namespace phenyl;

Logger engine::detail::ENGINE_LOGGER{"ENGINE", PHENYL_LOGGER};

class engine::detail::Engine {
private:
    //game::PhenylGameHolder gameObjHolder;
    graphics::PhenylGraphicsHolder graphicsHolder;
    //component::EntityComponentManager componentManager;
    //std::unique_ptr<component::EntitySerializer> entitySerializer;
    std::unique_ptr<component::PrefabManager> prefabManager;
    std::unique_ptr<game::LevelManager> levelManager;
    game::GameCamera gameCamera;
    game::GameInput gameInput;
    runtime::PhenylRuntime runtime;

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

engine::PhenylEngine::PhenylEngine () = default;

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
    InitLogging(app->properties.loggingProperties);

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

engine::detail::Engine::Engine (const ApplicationProperties& properties) : graphicsHolder(properties.graphicsProperties),
                                                                           runtime(component::EntityComponentManager{256}) {
    prefabManager = std::make_unique<component::PrefabManager>(&runtime.manager(), &runtime.serializer());
    levelManager = std::make_unique<game::LevelManager>(&runtime.manager(), &runtime.serializer());
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

    runtime.addResource<common::DebugRenderConfig>();
    runtime.addResource<graphics::UIManagerRes>(graphics.getUIManager());

    runtime.addPlugin<physics::PhysicsPlugin2D>();
    runtime.addPlugin<audio::AudioPlugin>();
    runtime.addPlugin<graphics::ProfileUiPlugin>();

    // TODO: move all to user
    //graphics.getTextureAtlas("sprite").ifPresent([&gameObj](auto& atlas){gameObj.setTextureIds(atlas);});
    graphics.addEntityLayer(&runtime.manager()); // TODO: unhackify
    graphics.getUIManager().addRenderLayer(graphics.tempGetGraphics(), graphics.getRenderer());

    //gameObjHolder.initGame(graphics, eventBus);

    //gameObjHolder.getGameObject().getGameInput().addInputSources(graphicsHolder.tempGetGraphics()->getInputSources());
    gameInput.addInputSources(graphicsHolder.tempGetGraphics()->getInputSources());
}

engine::detail::Engine::~Engine () {
    PHENYL_LOGI(ENGINE_LOGGER, "Shutting down!");
    prefabManager->clear();
    runtime.manager().clearAll();
}

graphics::PhenylGraphics engine::detail::Engine::getGraphics () const {
    return graphicsHolder.getGraphics();
}

void engine::detail::Engine::setupCallbacks () {
    graphicsHolder.tempGetGraphics()->setupWindowCallbacks();
}

component::EntityComponentManager& engine::detail::Engine::getComponentManager () {
    return runtime.manager();
}

const component::EntityComponentManager& engine::detail::Engine::getComponentManager () const {
    return runtime.manager();
}

component::EntitySerializer& engine::detail::Engine::getEntitySerializer () {
    return runtime.serializer();
}

void engine::detail::Engine::addDefaultSerialisers () {
    runtime.serializer().addSerializer<common::GlobalTransform2D>();
    runtime.serializer().addSerializer<common::TimedLifetime>();
}

game::GameCamera& engine::detail::Engine::getCamera () {
    return gameCamera;
}

game::GameInput& engine::detail::Engine::getInput () {
    return gameInput;
}

void engine::detail::Engine::addComponents () {
    runtime.manager().addComponent<common::GlobalTransform2D>();
    runtime.manager().addComponent<common::TimedLifetime>();

    graphicsHolder.tempGetGraphics()->addComponents(runtime.manager());
}

void engine::detail::Engine::dumpLevel (std::ostream& file) {
    levelManager->dump(file);
}

void engine::detail::Engine::gameloop (Application* app) {
    double deltaPhysicsFrame = 0.0f;
    auto graphics = graphicsHolder.getGraphics();
    PHENYL_LOGD(ENGINE_LOGGER, "Starting loop!");
    while (!graphics.shouldClose()) {
        PHENYL_TRACE(ENGINE_LOGGER, "Frame start");
        util::startProfileFrame();

        graphics.updateUI();
        gameInput.poll();

        double deltaTime = graphics.getDeltaTime();
        deltaPhysicsFrame += deltaTime * app->fixedTimeScale;

        util::startProfile("physics");
        while (deltaPhysicsFrame >= 1.0 / FIXED_FPS) {
            PHENYL_TRACE(ENGINE_LOGGER, "Physics frame start");
            fixedUpdate(app);
            deltaPhysicsFrame -= 1.0 / FIXED_FPS;
            PHENYL_TRACE(ENGINE_LOGGER, "Physics frame end");
        }
        util::endProfile();

        util::startProfile("graphics");
        update(app, deltaTime);
        render(app, deltaTime);
        util::endProfile();

        util::endProfileFrame();

        graphics.sync((int)app->targetFps); // TODO
        graphics.pollEvents();
        PHENYL_TRACE(ENGINE_LOGGER, "Frame end");
    }
}

void engine::detail::Engine::update (Application* app, double deltaTime) {
    PHENYL_TRACE(ENGINE_LOGGER, "Update start");
    app->update(deltaTime);
    graphicsHolder.getGraphics().frameUpdate(runtime.manager());
    common::TimedLifetime::Update(runtime.manager(), deltaTime); // TODO: put somewhere else

    runtime.pluginUpdate(deltaTime);
    PHENYL_TRACE(ENGINE_LOGGER, "Update end");
}

void engine::detail::Engine::fixedUpdate (Application* app) {
    PHENYL_TRACE(ENGINE_LOGGER, "Fixed update start");
    app->fixedUpdate(1.0 / FIXED_FPS);
    getCamera().updateCamera(getGraphics().getCamera());

    runtime.pluginFixedUpdate(1.0 / FIXED_FPS);

    runtime.pluginPhysicsUpdate(1.0 / FIXED_FPS);
    PHENYL_TRACE(ENGINE_LOGGER, "Fixed update end");
}

void engine::detail::Engine::render (Application* app, double deltaTime) {
    PHENYL_TRACE(ENGINE_LOGGER, "Render start");

    getGraphics().getUIManager().renderUI();
    runtime.pluginRender(deltaTime);

    getGraphics().render();
    PHENYL_TRACE(ENGINE_LOGGER, "Render end");
}

void engine::detail::Engine::setDebugRender (bool doRender) {
    runtime.resource<common::DebugRenderConfig>().doPhysicsRender = doRender;
}

void engine::detail::Engine::setProfileRender (bool doRender) {
    runtime.resource<common::DebugRenderConfig>().doProfileRender = doRender;
}
