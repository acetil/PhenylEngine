#pragma once

#include <memory>
#include <utility>

#include "graphics/graphics_headers.h"
#include "event/forward.h"

namespace graphics {
    class GLRenderer;
    class GLWindowCallbackCtx {
    public:
        GLWindowCallbackCtx (std::weak_ptr<event::EventBus> _eventBus, GLRenderer* _renderer) : eventBus{std::move(_eventBus)}, renderer{_renderer} {};
        std::weak_ptr<event::EventBus> eventBus;
        GLRenderer* renderer;
    };

    void setupGLWindowCallbacks (GLFWwindow* window);
    void setupGLWindowCallbacks (GLFWwindow* window, GLWindowCallbackCtx* ctx);
    void removeGLWindowCallbacks (GLFWwindow* window);
}
