#include <stdio.h>
#include <stdlib.h>

#include "graphics/graphics_headers.h"
#include "graphics/graphics_init.h"

int main (int argv, char* argc[]) {
    GLFWwindow* window = NULL;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        return GRAPHICS_INIT_FAILURE;
    }
    return EXIT_SUCCESS;
}