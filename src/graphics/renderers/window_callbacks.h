#include "graphics/graphics.h"
#include "renderer.h"
#include "event/event_bus.h"
#ifndef WINDOW_CALLBACKS_H
#define WINDOW_CALLBACKS_H
namespace graphics {
    struct WindowCallbackContext {
        Graphics* graphics;
        Renderer* renderer;
        std::weak_ptr<event::EventBus> eventBus;

    };
    void onMousePosChange (WindowCallbackContext* ctx, double windowX, double windowY, int windowSizeX, int windowSizeY);

}
#endif
