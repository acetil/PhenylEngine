#include <exception>
#include <fstream>

#include "component/component.h"

#include "engine/engine.h"
#include "engine/detail/loggers.h"
#include "component/component_serializer.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"
#include "graphics/ui/debug_ui.h"
#include "graphics/ui/ui_manager.h"
#include "graphics/ui/ui_plugin.h"

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
#include "graphics/graphics_plugin.h"
#include "graphics/particles/particle_plugin.h"
#include "common/plugins/timed_lifetime_plugin.h"
#include "common/plugins/core_plugin_2d.h"
#include "engine/plugins/level_plugin.h"
#include "engine/plugins/prefab_plugin.h"

#define FIXED_FPS 60.0

using namespace phenyl;

Logger engine::detail::ENGINE_LOGGER{"ENGINE", PHENYL_LOGGER};

class engine::detail::Engine {
private:
    //game::PhenylGameHolder gameObjHolder;
    //graphics::PhenylGraphicsHolder graphicsHolder;
    std::unique_ptr<graphics::detail::Graphics> graphics;
    //component::EntityComponentManager componentManager;
    //std::unique_ptr<component::EntitySerializer> entitySerializer;
    runtime::PhenylRuntime runtime;

    bool doDebugRender = false;
    bool doProfileRender = true;
public:
    Engine (const ApplicationProperties& properties);
    ~Engine();

    //[[nodiscard]] game::detail::GameObject::SharedPtr getGameObjectTemp () const;
    //[[nodiscard]] game::PhenylGame getGameObject () const;
    //[[nodiscard]] graphics::PhenylGraphics getGraphics () const;

    runtime::PhenylRuntime& getRuntime ();


    void gameloop (Application* app);
    void update (Application* app, double deltaTime);
    void fixedUpdate (Application* app);
    void render (Application* app, double deltaTime);
};


engine::PhenylEngine::PhenylEngine () = default;

engine::PhenylEngine::~PhenylEngine () = default;


void engine::PhenylEngine::exec (Application* app) {
    InitLogging(app->properties.loggingProperties);

    internal = std::make_unique<engine::detail::Engine>(app->properties);
    app->init();
    internal->gameloop(app);
    app->shutdown();
    internal = nullptr;
}

runtime::PhenylRuntime& phenyl::engine::PhenylEngine::getRuntime () {
    return internal->getRuntime();
}

engine::detail::Engine::Engine (const ApplicationProperties& properties) : graphics(graphics::MakeGraphics(properties.graphicsProperties)),
                                                                           runtime(component::EntityComponentManager{256}) {

    runtime.addResource<common::DebugRenderConfig>();
    runtime.addResource(graphics.get());

    runtime.addPlugin<LevelPlugin>();
    runtime.addPlugin<PrefabPlugin>();
    runtime.addPlugin<common::Core2DPlugin>();
    runtime.addPlugin<graphics::GraphicsPlugin>();
    runtime.addPlugin<graphics::Particle2DPlugin>();
    runtime.addPlugin<graphics::UIPlugin>();
    runtime.addPlugin<physics::PhysicsPlugin2D>();
    runtime.addPlugin<audio::AudioPlugin>();
    runtime.addPlugin<graphics::ProfileUiPlugin>();
    runtime.addPlugin<game::GameInputPlugin>();
    runtime.addPlugin<common::TimedLifetimePlugin>();

    runtime.pluginPostInit();
}

engine::detail::Engine::~Engine () {
    PHENYL_LOGI(ENGINE_LOGGER, "Shutting down!");
    runtime.shutdown();
}

runtime::PhenylRuntime& engine::detail::Engine::getRuntime () {
    return runtime;
}

void engine::detail::Engine::gameloop (Application* app) {
    double deltaPhysicsFrame = 0.0f;
    PHENYL_LOGD(ENGINE_LOGGER, "Starting loop!");
    while (!graphics->shouldClose()) {
        PHENYL_TRACE(ENGINE_LOGGER, "Frame start");
        util::startProfileFrame();

        runtime.pluginFrameBegin();

        double deltaTime = graphics->getDeltaTime();
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

        graphics->sync((int)app->targetFps); // TODO
        graphics->pollEvents();
        PHENYL_TRACE(ENGINE_LOGGER, "Frame end");
    }
}

void engine::detail::Engine::update (Application* app, double deltaTime) {
    PHENYL_TRACE(ENGINE_LOGGER, "Update start");
    app->update(deltaTime);

    runtime.pluginUpdate(deltaTime);
    PHENYL_TRACE(ENGINE_LOGGER, "Update end");
}

void engine::detail::Engine::fixedUpdate (Application* app) {
    PHENYL_TRACE(ENGINE_LOGGER, "Fixed update start");
    app->fixedUpdate(1.0 / FIXED_FPS);

    runtime.pluginFixedUpdate(1.0 / FIXED_FPS);
    runtime.pluginPhysicsUpdate(1.0 / FIXED_FPS);

    PHENYL_TRACE(ENGINE_LOGGER, "Fixed update end");
}

void engine::detail::Engine::render (Application* app, double deltaTime) {
    PHENYL_TRACE(ENGINE_LOGGER, "Render start");

    runtime.pluginRender(deltaTime);

    graphics->render(); // TODO: remove dependency on graphics once refactored
    PHENYL_TRACE(ENGINE_LOGGER, "Render end");
}