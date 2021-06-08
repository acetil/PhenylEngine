#include "graphics/graphics.h"
#include "renderer.h"
#include "event/event.h"
#ifndef WINDOW_CALLBACKS_H
#define WINDOW_CALLBACKS_H
namespace graphics {
    struct WindowCallbackContext {
        Graphics* graphics;
        Renderer* renderer;
        event::EventBus::SharedPtr eventBus;
    };
    void onMousePosChange (WindowCallbackContext* ctx, double windowX, double windowY, int windowSizeX, int windowSizeY);

}
#endif
