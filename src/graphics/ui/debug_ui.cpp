#include "debug_ui.h"

#include <utility>
#include "component/view/debug_game_view.h"

#include "util/profiler.h"

#include "event/events/debug/profiler_change.h"

using namespace graphics;

static double avgGraphicsTime = 0;
static double avgPhysicsTime = 0;
static double avgFrameTime = 0;

static double totGraphicsTime = 0;
static double totPhysicsTime = 0;
static double totFrameTime = 0;

static int smoothFrames = 0;

static bool doDisplayProfiler = true;

void graphics::renderDebugUi (game::GameObject::SharedPtr gameObject, UIManager& uiManager) {
    view::DebugGameView debugView(std::move(gameObject));

    if (smoothFrames % 30 == 0) {
        avgFrameTime = totFrameTime / 30;
        avgGraphicsTime = totGraphicsTime / 30;
        avgPhysicsTime = totPhysicsTime / 30;

        totFrameTime = 0;
        totGraphicsTime = 0;
        totPhysicsTime = 0;
    }

    totFrameTime += util::getProfileFrameTime();
    totGraphicsTime += util::getProfileTime("graphics");
    totPhysicsTime += util::getProfileTime("physics");

    smoothFrames++;
    if (doDisplayProfiler) {
        uiManager.renderText("noto-serif", "physics: " + std::to_string(avgPhysicsTime * 1000) + "ms", 14, 5, 15);
        uiManager.renderText("noto-serif", "graphics: " + std::to_string(avgGraphicsTime * 1000) + "ms", 14, 5, 30);
        uiManager.renderText("noto-serif", "frame time: " + std::to_string(avgFrameTime * 1000) + "ms", 14, 5, 45);
        //uiManager.renderText("noto-serif", "a", 288 * 2, 20, 600);
    }
}

void handleProfilerChange(const event::ProfilerChangeEvent& event) {
    doDisplayProfiler = event.doDisplay.value_or(doDisplayProfiler);
}

void graphics::addDebugEventHandlers (event::EventBus::SharedPtr bus) {
    bus->subscribeHandler(handleProfilerChange);
}
