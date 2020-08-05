#include <stdlib.h>

#include "graphics/graphics_headers.h"
#include "graphics/graphics_init.h"

#include "game/gameloop.h"

#include "logging/logging_internal.h"

int main (int argv, char* argc[]) {
    // TODO: move to exceptions
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        internal::log_internal(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    graphics::Graphics* graphics = nullptr;
    if (graphics::initGraphics(window, &graphics) != GRAPHICS_INIT_SUCCESS) {
        internal::log_internal(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    internal::log_internal(LEVEL_INFO, "MAIN", "Successfully initialised graphics");
    game::gameloop(graphics);
    internal::log_internal(LEVEL_INFO, "MAIN", "Shutting down!");
    graphics::destroyGraphics(graphics);
    return EXIT_SUCCESS;
}