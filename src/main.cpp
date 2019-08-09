#include <stdio.h>
#include <stdlib.h>

#include "graphics/graphics_headers.h"
#include "graphics/graphics_init.h"
#include "graphics/graphics.h"

#include "game/gameloop.h"

#include "logging/logging.h"

int main (int argv, char* argc[]) {
    GLFWwindow* window = NULL;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logging::log(LEVEL_INFO, "Window init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    graphics::Graphics* graphics = NULL;
    if (graphics::initGraphics(window, &graphics) != GRAPHICS_INIT_SUCCESS) {
        logging::log(LEVEL_INFO, "Graphics init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    logging::log(LEVEL_INFO, "Successfully initialised graphics");
    game::gameloop(graphics);
    logging::log(LEVEL_INFO, "Shutting down!");
    return EXIT_SUCCESS;
}