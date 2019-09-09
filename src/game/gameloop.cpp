#include "gameloop.h"
#include "game_init.h"
#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"
#include "key_input.h"
#include "key_defaults.h"

#define TARGET_FPS 60

using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    GameObject* gameObject = initGame(graphics);
    AbstractEntity* player = gameObject->createNewEntityInstance("test_entity", 0, 0);
    KeyboardInput* keyInput = new KeyboardInput(graphics);
    setupMovementKeys(keyInput, &player);
    float deltaTime = 0.0f;
    graphics->startTimer(TARGET_FPS);
    logging::log(LEVEL_DEBUG, "Starting loop");
    while (!graphics->shouldClose()) {
        deltaTime = graphics->getDeltaTime();
        keyInput->handleKeyPresses();
        gameObject->updateEntities(deltaTime);
        gameObject->updateEntityPositions(deltaTime);

        gameObject->renderEntities(graphics);
        graphics->render();
        graphics->pollEvents();
    }
    return 0;
}