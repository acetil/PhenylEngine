#include "gameloop.h"
#include "graphics/graphics.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"

using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    while (!graphics->shouldClose()) {
        graphics->drawTexSquare(0, 0, 1, graphics->getSpriteTextureId("test1"));
        graphics->render();
        graphics->pollEvents();
    }
    return 0;
}