#pragma once

#include <memory>
#include <utility>

#include "graphics/graphics_headers.h"

namespace graphics {
    class GLRenderer;
    struct GLWindowCallbackCtx {
        GLRenderer* renderer;
    };

    void setupGLWindowCallbacks (GLFWwindow* window, GLWindowCallbackCtx* ctx);
    void removeGLWindowCallbacks (GLFWwindow* window);
}
