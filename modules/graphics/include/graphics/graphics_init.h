#pragma once

#include "graphics/graphics_headers.h"
#include "graphics/graphics.h"
#include "graphics.h"

#include "graphics_properties.h"

#define DEFAULT_WINDOW_X 800
#define DEFAULT_WINDOW_Y 600
#define DEFAULT_WINDOW_NAME "Action game"
#define GRAPHICS_INIT_SUCCESS 0
#define GRAPHICS_INIT_FAILURE -1
namespace phenyl::graphics {
    int initWindow (GLFWwindow** windowPtr, const GraphicsProperties& properties);
    std::unique_ptr<detail::Graphics> initGraphics (GLFWwindow* window);
}
