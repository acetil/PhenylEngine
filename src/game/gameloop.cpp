#include "gameloop.h"
#include "game_init.h"
#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"

using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    GameObject* gameObject = initGame(graphics);
    gameObject->createNewEntityInstance("test_entity", 0, 0);
    while (!graphics->shouldClose()) {
        //gameObject->updateEntities();
        //gameObject->updateEntityPositions();
        gameObject->renderEntities(graphics);
        graphics->render();
        graphics->pollEvents();
    }
    return 0;
}