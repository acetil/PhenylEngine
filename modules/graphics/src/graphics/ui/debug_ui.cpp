#include "graphics/ui/debug_ui.h"

#include <utility>
#include <queue>

#include "util/profiler.h"
#include "util/smooth_queue.h"
#include "common/debug.h"

using namespace phenyl;

std::string_view graphics::ProfileUiPlugin::getName () const noexcept {
    return "ProfileUiPlugin";
}

void graphics::ProfileUiPlugin::init (runtime::PhenylRuntime& runtime) {}

void graphics::ProfileUiPlugin::update (runtime::PhenylRuntime& runtime, double deltaTime) {
    deltaTimeQueue.pushPop((float)deltaTime);
    frameQueue.pushPop(util::getProfileFrameTime());
    graphicsQueue.pushPop(util::getProfileTime("graphics"));
    physicsQueue.pushPop(util::getProfileTime("physics"));
}

void graphics::ProfileUiPlugin::render (runtime::PhenylRuntime& runtime) {
    if (!runtime.resource<common::DebugRenderConfig>().doProfileRender) {
        return;
    }

    auto& uiManager = runtime.resource<UIManagerRes>().manager;

    uiManager.renderText("noto-serif", "physics: " + std::to_string(physicsQueue.getSmoothed() * 1000) + "ms", 14, 5,
                         15);
    uiManager.renderText("noto-serif", "graphics: " + std::to_string(graphicsQueue.getSmoothed() * 1000) + "ms", 14, 5,
                         30);
    uiManager.renderText("noto-serif", "frame time: " + std::to_string(frameQueue.getSmoothed() * 1000) + "ms", 14, 5,
                         45);
    uiManager.renderText("noto-serif", std::to_string(1.0f / deltaTimeQueue.getSmoothed()) + " fps",
                         14, 700, 15, {0.0f, 1.0f, 0.0f});
}
