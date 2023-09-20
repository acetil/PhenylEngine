#pragma once

#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"
#include "graphics.h"

#define DEFAULT_WINDOW_X 800
#define DEFAULT_WINDOW_Y 600
#define DEFAULT_WINDOW_NAME "Action game"
#define GRAPHICS_INIT_SUCCESS 0
#define GRAPHICS_INIT_FAILURE -1
namespace phenyl::graphics {
    int initWindow (GLFWwindow** windowPtr);
    int initGraphics (GLFWwindow* window, detail::Graphics::SharedPtr& graphicsNew);
    void destroyGraphics (const detail::Graphics::SharedPtr& graphics);
}
