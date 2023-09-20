#include "graphics/ui/debug_ui.h"

#include <utility>
#include <queue>

#include "util/profiler.h"
#include "util/smooth_queue.h"

using namespace phenyl;

static bool doDisplayProfiler = true;

static util::SmoothQueue<double, 30> graphicsQueue;
static util::SmoothQueue<double, 30> physicsQueue;
static util::SmoothQueue<double, 30> frameQueue;
static util::SmoothQueue<float, 30> deltaTimeQueue;

void graphics::renderDebugUi (UIManager& uiManager, float deltaTime) {

    deltaTimeQueue.pushPop(deltaTime);
    frameQueue.pushPop(util::getProfileFrameTime());
    graphicsQueue.pushPop(util::getProfileTime("graphics"));
    physicsQueue.pushPop(util::getProfileTime("physics"));

    uiManager.renderText("noto-serif", "physics: " + std::to_string(physicsQueue.getSmoothed() * 1000) + "ms", 14, 5,
                         15);
    uiManager.renderText("noto-serif", "graphics: " + std::to_string(graphicsQueue.getSmoothed() * 1000) + "ms", 14, 5,
                         30);
    uiManager.renderText("noto-serif", "frame time: " + std::to_string(frameQueue.getSmoothed() * 1000) + "ms", 14, 5,
                         45);//uiManager.renderText("noto-serif", "a", 288 * 2, 20, 600);
    uiManager.renderText("noto-serif", std::to_string(1.0f / deltaTimeQueue.getSmoothed()) + " fps",
                         14, 700, 15, {0.0f, 1.0f, 0.0f});
}