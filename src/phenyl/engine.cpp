#include "phenyl/engine.h"

#include "core/runtime.h"
#include "engine_clock.h"
#include "graphics/backend/renderer.h"
#include "graphics/phenyl_graphics.h"
#include "logging/logging.h"
#include "plugins/app_plugin.h"
#include "util/profiler.h"

#include <chrono>
#include <exception>
#include <fstream>
#include <thread>

#define FIXED_FPS 60.0

using namespace phenyl;

static Logger LOGGER{"ENGINE", PHENYL_LOGGER};

class engine::Engine {
public:
    explicit Engine (const ApplicationProperties& properties) :
        m_renderer{graphics::MakeVulkanRenderer(properties.m_graphics)},
        m_runtime(),
        m_clock{std::chrono::duration_cast<EngineClock::Duration>(std::chrono::duration<double>(1.0 / FIXED_FPS))} {}

    ~Engine () {
        PHENYL_LOGI(LOGGER, "Shutting down!");
        m_runtime.shutdown();
        m_renderer->clearLayers();
    }

    core::PhenylRuntime& getRuntime () {
        return m_runtime;
    }

    void init (std::unique_ptr<ApplicationBase> app) {
        m_runtime.addResource(m_renderer.get());
        m_runtime.addResource<core::Clock>(&m_clock);

        m_runtime.registerPlugin(std::make_unique<AppPlugin>(std::move(app)));

        m_runtime.runPostInit();
    }

    void gameloop (ApplicationBase* app) {
        // double deltaPhysicsFrame = 0.0f;
        PHENYL_LOGD(LOGGER, "Starting loop!");
        while (!m_renderer->getViewport().shouldClose()) {
            PHENYL_TRACE(LOGGER, "Frame start");
            util::startProfileFrame();

            m_runtime.runFrameBegin();

            util::startProfile("physics");
            while (m_clock.startFixedFrame()) {
                PHENYL_TRACE(LOGGER, "Physics frame start");
                fixedUpdate();
                // m_fixedTimeSlop -= 1.0 / FIXED_FPS;
                PHENYL_TRACE(LOGGER, "Physics frame end");
            }
            util::endProfile();

            util::startProfile("graphics");
            m_clock.startVariableFrame();
            update(m_clock.deltaTime());
            render(m_clock.deltaTime());
            util::endProfile();

            util::endProfileFrame();

            sync(app); // TODO
            m_renderer->getViewport().poll();
            PHENYL_TRACE(LOGGER, "Frame end");
        }
    }

    void update (double deltaTime) {
        PHENYL_TRACE(LOGGER, "Update start");
        m_runtime.runVariableTimestep(deltaTime);
        PHENYL_TRACE(LOGGER, "Update end");
    }

    void fixedUpdate () {
        PHENYL_TRACE(LOGGER, "Fixed update start");
        m_runtime.runFixedTimestep(1.0 / FIXED_FPS);
        PHENYL_TRACE(LOGGER, "Fixed update end");
    }

    void render (double deltaTime) {
        PHENYL_TRACE(LOGGER, "Render start");
        m_runtime.runRender();
        m_renderer->render();
        PHENYL_TRACE(LOGGER, "Render end");
    }

    void sync (ApplicationBase* app) {
        std::chrono::duration<double> targetFrameTime{1.0 / app->getTargetFps()};
        while (!m_renderer->getViewport().shouldClose() && m_clock.frameTime() < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        m_clock.advance(app->getFixedTimeScale());
    }

private:
    std::unique_ptr<graphics::Renderer> m_renderer;
    core::PhenylRuntime m_runtime;
    EngineClock m_clock;
};

PhenylEngine::PhenylEngine (const logging::LoggingProperties& properties) {
    InitLogging(properties);
}

PhenylEngine::~PhenylEngine () {
    ShutdownLogging();
}

void PhenylEngine::run (std::unique_ptr<engine::ApplicationBase> app) {
    // InitLogging(app->getProperties().loggingProperties);

    auto* appPtr = app.get();
    m_internal = std::make_unique<engine::Engine>(app->getProperties());
    m_internal->init(std::move(app));
    m_internal->gameloop(appPtr);
    m_internal = nullptr;
}
