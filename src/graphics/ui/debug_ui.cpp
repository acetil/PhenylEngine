#include "debug_ui.h"
#include "component/view/debug_game_view.h"

#include "util/profiler.h"

using namespace graphics;

static double avgGraphicsTime = 0;
static double avgPhysicsTime = 0;
static double avgFrameTime = 0;

static double totGraphicsTime = 0;
static double totPhysicsTime = 0;
static double totFrameTime = 0;

static int smoothFrames = 0;

void graphics::renderDebugUi (game::GameObject* gameObject, UIManager& uiManager) {
    view::DebugGameView debugView(gameObject);

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

    uiManager.renderText("noto-serif", "physics: " + std::to_string(avgPhysicsTime * 1000) + "ms", 11, 20, 20);
    uiManager.renderText("noto-serif", "graphics: " + std::to_string(avgGraphicsTime * 1000) + "ms", 11, 20, 45);
    uiManager.renderText("noto-serif", "frame time: " + std::to_string(avgFrameTime * 1000) + "ms", 11, 20, 70);
}