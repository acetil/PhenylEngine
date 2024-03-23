#include <exception>
#include <fstream>

#include "component/component.h"

#include "engine/engine.h"
#include "graphics/graphics.h"

#include "logging/logging.h"
#include "util/profiler.h"

#include "runtime/runtime.h"
#include "engine/plugins/app_plugin.h"

#define FIXED_FPS 60.0

using namespace phenyl;

static Logger LOGGER{"ENGINE", PHENYL_LOGGER};

class engine::detail::Engine {
private:
    std::unique_ptr<graphics::detail::Graphics> graphics; // TODO: move to plugin
    runtime::PhenylRuntime runtime;
public:
    Engine (const ApplicationProperties& properties) : graphics(graphics::MakeGraphics(properties.graphicsProperties)), runtime(component::EntityComponentManager{256}) {}
    ~Engine() {
        PHENYL_LOGI(LOGGER, "Shutting down!");
        runtime.shutdown();
    }

    runtime::PhenylRuntime& getRuntime () {
        return runtime;
    }

    void init (ApplicationBase* app) {
        runtime.addResource(graphics.get()); // TODO: remove

        runtime.registerPlugin(std::make_unique<AppPlugin>(app));

        runtime.pluginPostInit();
    }

    void gameloop (ApplicationBase* app) {
        double deltaPhysicsFrame = 0.0f;
        PHENYL_LOGD(LOGGER, "Starting loop!");
        while (!graphics->shouldClose()) {
            PHENYL_TRACE(LOGGER, "Frame start");
            util::startProfileFrame();

            runtime.pluginFrameBegin();

            double deltaTime = graphics->getDeltaTime();
            deltaPhysicsFrame += deltaTime * app->getFixedTimeScale();

            util::startProfile("physics");
            while (deltaPhysicsFrame >= 1.0 / FIXED_FPS) {
                PHENYL_TRACE(LOGGER, "Physics frame start");
                fixedUpdate();
                deltaPhysicsFrame -= 1.0 / FIXED_FPS;
                PHENYL_TRACE(LOGGER, "Physics frame end");
            }
            util::endProfile();

            util::startProfile("graphics");
            update(deltaTime);
            render(deltaTime);
            util::endProfile();

            util::endProfileFrame();

            graphics->sync((int)app->getTargetFps()); // TODO
            graphics->pollEvents();
            PHENYL_TRACE(LOGGER, "Frame end");
        }
    }

    void update (double deltaTime) {
        PHENYL_TRACE(LOGGER, "Update start");
        runtime.pluginUpdate(deltaTime);
        PHENYL_TRACE(LOGGER, "Update end");
    }
    void fixedUpdate () {
        PHENYL_TRACE(LOGGER, "Fixed update start");
        runtime.pluginFixedUpdate(1.0 / FIXED_FPS);
        runtime.pluginPhysicsUpdate(1.0 / FIXED_FPS);

        PHENYL_TRACE(LOGGER, "Fixed update end");
    }

    void render (double deltaTime) {
        PHENYL_TRACE(LOGGER, "Render start");

        runtime.pluginRender(deltaTime);

        graphics->render(); // TODO: remove dependency on graphics once refactored
        PHENYL_TRACE(LOGGER, "Render end");
    }
};


engine::PhenylEngine::PhenylEngine () = default;

engine::PhenylEngine::~PhenylEngine () = default;

void engine::PhenylEngine::exec (ApplicationBase* app) {
    InitLogging(app->getProperties().loggingProperties);

    internal = std::make_unique<engine::detail::Engine>(app->getProperties());
    internal->init(app);
    internal->gameloop(app);
    internal = nullptr;
}