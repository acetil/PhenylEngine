#include <chrono>
#include <thread>
#include <exception>
#include <fstream>

#include "component/component.h"

#include "phenyl/engine.h"

#include "logging/logging.h"
#include "util/profiler.h"

#include "runtime/runtime.h"
#include "plugins/app_plugin.h"
#include "graphics/renderers/renderer.h"
#include "graphics/phenyl_graphics.h"

#define FIXED_FPS 60.0

using namespace phenyl;

static Logger LOGGER{"ENGINE", PHENYL_LOGGER};

class engine::Engine {
private:
    std::unique_ptr<graphics::Renderer> renderer;
    runtime::PhenylRuntime runtime;

    double lastTime;
    double deltaTime{0.0};
    double fixedTimeSlop{0.0};
public:
    explicit Engine (const ApplicationProperties& properties) : renderer{graphics::MakeGLRenderer(properties.graphicsProperties)}, runtime(component::EntityComponentManager{256}), lastTime{renderer->getCurrentTime()} {}

    ~Engine() {
        PHENYL_LOGI(LOGGER, "Shutting down!");
        runtime.shutdown();
        renderer->clearLayers();
    }

    runtime::PhenylRuntime& getRuntime () {
        return runtime;
    }

    void init (ApplicationBase* app) {
        runtime.addResource(renderer.get());

        runtime.registerPlugin(std::make_unique<AppPlugin>(app));

        runtime.pluginPostInit();
    }

    void gameloop (ApplicationBase* app) {
        //double deltaPhysicsFrame = 0.0f;
        PHENYL_LOGD(LOGGER, "Starting loop!");
        while (!renderer->getViewport().shouldClose()) {
            PHENYL_TRACE(LOGGER, "Frame start");
            util::startProfileFrame();

            runtime.pluginFrameBegin();

            //double deltaTime = graphics->getDeltaTime();
            fixedTimeSlop += deltaTime * app->getFixedTimeScale();

            util::startProfile("physics");
            while (fixedTimeSlop >= 1.0 / FIXED_FPS) {
                PHENYL_TRACE(LOGGER, "Physics frame start");
                fixedUpdate();
                fixedTimeSlop -= 1.0 / FIXED_FPS;
                PHENYL_TRACE(LOGGER, "Physics frame end");
            }
            util::endProfile();

            util::startProfile("graphics");
            update(deltaTime);
            render(deltaTime);
            util::endProfile();

            util::endProfileFrame();

            sync(app->getTargetFps()); // TODO
            renderer->getViewport().poll();
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

        renderer->render();
        PHENYL_TRACE(LOGGER, "Render end");
    }

    void sync (double fps) {
        while (renderer->getCurrentTime() - lastTime < 1.0 / (fps)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        double currTime = renderer->getCurrentTime();
        deltaTime = currTime - lastTime;
        lastTime = currTime;
    }
};


PhenylEngine::PhenylEngine () = default;

PhenylEngine::~PhenylEngine () = default;

void PhenylEngine::exec (engine::ApplicationBase* app) {
    InitLogging(app->getProperties().loggingProperties);

    internal = std::make_unique<engine::Engine>(app->getProperties());
    internal->init(app);
    internal->gameloop(app);
    internal = nullptr;
}