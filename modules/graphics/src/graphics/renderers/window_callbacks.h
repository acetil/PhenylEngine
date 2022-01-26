#pragma once

#include "graphics/graphics.h"
#include "graphics/renderers/renderer.h"
#include "event/event_bus.h"

namespace graphics {
    struct WindowCallbackContext {
        Graphics* graphics;
        Renderer* renderer;
        std::weak_ptr<event::EventBus> eventBus;

    };
    void onMousePosChange (WindowCallbackContext* ctx, double windowX, double windowY, int windowSizeX, int windowSizeY);

}
