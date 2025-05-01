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
public:
    explicit Engine (const ApplicationProperties& properties) : m_renderer{graphics::MakeVulkanRenderer(properties.m_graphics)}, m_runtime(), m_lastTime{m_renderer->getCurrentTime()} {}

    ~Engine() {
        PHENYL_LOGI(LOGGER, "Shutting down!");
        m_runtime.shutdown();
        m_renderer->clearLayers();
    }

    core::PhenylRuntime& getRuntime () {
        return m_runtime;
    }

    void init (std::unique_ptr<ApplicationBase> app) {
        m_runtime.addResource(m_renderer.get());

        m_runtime.registerPlugin(std::make_unique<AppPlugin>(std::move(app)));

        m_runtime.runPostInit();
    }

    void gameloop (ApplicationBase* app) {
        //double deltaPhysicsFrame = 0.0f;
        PHENYL_LOGD(LOGGER, "Starting loop!");
        while (!m_renderer->getViewport().shouldClose()) {
            PHENYL_TRACE(LOGGER, "Frame start");
            util::startProfileFrame();

            m_runtime.runFrameBegin();

            //double deltaTime = graphics->getDeltaTime();
            m_fixedTimeSlop += m_deltaTime * app->getFixedTimeScale();

            util::startProfile("physics");
            while (m_fixedTimeSlop >= 1.0 / FIXED_FPS) {
                PHENYL_TRACE(LOGGER, "Physics frame start");
                fixedUpdate();
                m_fixedTimeSlop -= 1.0 / FIXED_FPS;
                PHENYL_TRACE(LOGGER, "Physics frame end");
            }
            util::endProfile();

            util::startProfile("graphics");
            update(m_deltaTime);
            render(m_deltaTime);
            util::endProfile();

            util::endProfileFrame();

            sync(app->getTargetFps()); // TODO
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

    void sync (double fps) {
        while (m_renderer->getCurrentTime() - m_lastTime < 1.0 / (fps)) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        double currTime = m_renderer->getCurrentTime();
        m_deltaTime = currTime - m_lastTime;
        m_lastTime = currTime;
    }

private:
    std::unique_ptr<graphics::Renderer> m_renderer;
    core::PhenylRuntime m_runtime;

    double m_lastTime;
    double m_deltaTime{0.0};
    double m_fixedTimeSlop{0.0};
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
    m_internal = std::make_unique<engine::Engine>(app->getProperties());
    m_internal->init(std::move(app));
    m_internal->gameloop(appPtr);
    m_internal = nullptr;
}