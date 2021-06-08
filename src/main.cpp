#include <stdlib.h>

#include "graphics/graphics_headers.h"
#include "graphics/graphics_init.h"

#include "game/gameloop.h"

#include "logging/logging_internal.h"
#include "util/format.h"

int main (int argv, char* argc[]) {
    // TODO: move to exceptions
    internal::log_internal(LEVEL_DEBUG, "MAIN", "Started game!");
    internal::log_internal(LEVEL_DEBUG, "MAIN", util::format("Test format: {} {} {1}", 4, 3.8, "abc").c_str());
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        internal::log_internal(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    graphics::Graphics::SharedPtr graphics;
    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        internal::log_internal(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    internal::log_internal(LEVEL_INFO, "MAIN", "Successfully initialised graphics");
    game::gameloop(graphics);
    internal::log_internal(LEVEL_INFO, "MAIN", "Shutting down!");
    graphics::destroyGraphics(graphics);
    return EXIT_SUCCESS;
}