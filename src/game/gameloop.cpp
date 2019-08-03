#include "gameloop.h"
#include "graphics/graphics.h"

#include "logging/logging.h"

using namespace game; 

int game::gameloop (graphics::Graphics graphics) {
    while (!graphics.shouldClose()) {
        graphics.render();
        graphics.pollEvents();
    }
    return 0;
}