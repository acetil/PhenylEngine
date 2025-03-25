#include <chrono>
#include <exception>
#include <fstream>
#include <thread>

#include "graphics/phenyl_graphics.h"
#include "graphics/backend/renderer.h"
#include "logging/logging.h"
#include "plugins/app_plugin.h"
#include "core/runtime.h"
#include "util/profiler.h"

#include "phenyl/engine.h"

#define FIXED_FPS 60.0

using namespace phenyl;

static Logger LOGGER{"ENGINE", PHENYL_LOGGER};

class engine::Engine {
private:
    std::unique_ptr<graphics::Renderer> renderer;
    core::PhenylRuntime runtime;

    double lastTime;
    double deltaTime{0.0};
    double fixedTimeSlop{0.0};
public:
    explicit Engine (const ApplicationProperties& properties) : renderer{graphics::MakeGLRenderer(properties.graphicsProperties)}, runtime(), lastTime{renderer->getCurrentTime()} {}

    ~Engine() {
        PHENYL_LOGI(LOGGER, "Shutting down!");
        runtime.shutdown();
        renderer->clearLayers();
    }

    core::PhenylRuntime& getRuntime () {
        return runtime;
    }

    void init (std::unique_ptr<ApplicationBase> app) {
        runtime.addResource(renderer.get());

        runtime.registerPlugin(std::make_unique<AppPlugin>(std::move(app)));

        runtime.runPostInit();
    }

    void gameloop (ApplicationBase* app) {
        //double deltaPhysicsFrame = 0.0f;
        PHENYL_LOGD(LOGGER, "Starting loop!");
        while (!renderer->getViewport().shouldClose()) {
            PHENYL_TRACE(LOGGER, "Frame start");
            util::startProfileFrame();

            runtime.runFrameBegin();

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
        runtime.runVariableTimestep(deltaTime);
        PHENYL_TRACE(LOGGER, "Update end");
    }
    void fixedUpdate () {
        PHENYL_TRACE(LOGGER, "Fixed update start");
        runtime.runFixedTimestep(1.0 / FIXED_FPS);
        PHENYL_TRACE(LOGGER, "Fixed update end");
    }

    void render (double deltaTime) {
        PHENYL_TRACE(LOGGER, "Render start");
        runtime.runRender();
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


PhenylEngine::PhenylEngine (const logging::LoggingProperties& properties) {
    InitLogging(properties);
}

PhenylEngine::~PhenylEngine () {
    ShutdownLogging();
}

void PhenylEngine::run (std::unique_ptr<engine::ApplicationBase> app) {
    //InitLogging(app->getProperties().loggingProperties);

    auto* appPtr = app.get();
    internal = std::make_unique<engine::Engine>(app->getProperties());
    internal->init(std::move(app));
    internal->gameloop(appPtr);
    internal = nullptr;
}