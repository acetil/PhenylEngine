#include "graphics/ui/debug_ui.h"

#include <utility>
#include <queue>
#include "component/view/debug_game_view.h"

#include "util/profiler.h"
#include "util/smooth_queue.h"

#include "event/events/debug/profiler_change.h"
#include "game_object.h"

using namespace graphics;

static bool doDisplayProfiler = true;

static util::SmoothQueue<double, 30> graphicsQueue;
static util::SmoothQueue<double, 30> physicsQueue;
static util::SmoothQueue<double, 30> frameQueue;
static util::SmoothQueue<float, 30> deltaTimeQueue;

void graphics::renderDebugUi (game::detail::GameObject::SharedPtr gameObject, UIManager& uiManager, float deltaTime) {

    deltaTimeQueue.pushPop(deltaTime);
    frameQueue.pushPop(util::getProfileFrameTime());
    graphicsQueue.pushPop(util::getProfileTime("graphics"));
    physicsQueue.pushPop(util::getProfileTime("physics"));

    view::DebugGameView debugView(std::move(gameObject));

    if (doDisplayProfiler) {
        uiManager.renderText("noto-serif", "physics: " + std::to_string(physicsQueue.getSmoothed() * 1000) + "ms", 14, 5, 15);
        uiManager.renderText("noto-serif", "graphics: " + std::to_string(graphicsQueue.getSmoothed() * 1000) + "ms", 14, 5, 30);
        uiManager.renderText("noto-serif", "frame time: " + std::to_string(frameQueue.getSmoothed() * 1000) + "ms", 14, 5, 45);
        //uiManager.renderText("noto-serif", "a", 288 * 2, 20, 600);
        uiManager.renderText("noto-serif", std::to_string(1.0f / deltaTimeQueue.getSmoothed()) + " fps",
                             14, 700, 15, {0.0f, 1.0f, 0.0f});
    }
}

void handleProfilerChange(const event::ProfilerChangeEvent& event) {
    doDisplayProfiler = event.doDisplay.value_or(doDisplayProfiler);
}

void graphics::addDebugEventHandlers (const event::EventBus::SharedPtr& bus) {
    bus->subscribeHandler(handleProfilerChange);
}
