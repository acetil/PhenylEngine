#include "gameloop.h"
#include "game_init.h"
#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"
#include "key_input.h"
#include "key_defaults.h"


using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    GameObject* gameObject = initGame(graphics);
    AbstractEntity* player = gameObject->createNewEntityInstance("test_entity", 0, 0);
    KeyboardInput* keyInput = new KeyboardInput(graphics);
    float deltaTime = 0.0f;
    
    while (!graphics->shouldClose()) {
        //gameObject->updateEntities();
        //gameObject->updateEntityPositions();
        gameObject->renderEntities(graphics);
        graphics->render();
        graphics->pollEvents();
    }
    return 0;
}