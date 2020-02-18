#include "gameloop.h"
#include "game_init.h"
#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"
#include "key_input.h"
#include "key_defaults.h"
#include "map/map_reader.h"

#define TARGET_FPS 60
#define PHYSICS_FPS 60
using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    GameObject* gameObject = initGame(graphics);
    AbstractEntity* player = gameObject->createNewEntityInstance("test_entity", 0, 0);
    logging::log(LEVEL_INFO, "Created player");
    KeyboardInput* keyInput = getKeyboardInput(graphics);
    setupMovementKeys(keyInput, gameObject->getEventBus());
    float deltaTime = 0.0f;
    float deltaPhysicsFrame = 0.0f;
    float timeSinceFpsUpdate = 0.0f;
    int frames = 0;
    Map* map = readMap("resources/maps/testmap.acmp", gameObject);
    map->initGraphicsData(graphics, "default");
    graphics->startTimer(TARGET_FPS);
    logging::log(LEVEL_DEBUG, "Starting loop");
    while (!graphics->shouldClose()) {
        deltaTime = graphics->getDeltaTime();
        deltaPhysicsFrame += deltaTime;
        timeSinceFpsUpdate += deltaTime;
        if (timeSinceFpsUpdate >= 1.0f) {
            logging::logf(LEVEL_DEBUG, "Done %d frames in %f second(s), with an average fps of %f", frames, 
            timeSinceFpsUpdate, frames / timeSinceFpsUpdate);
            timeSinceFpsUpdate = 0.0f;
            frames = 0;
        }
        keyInput->handleKeyPresses();
        while (deltaPhysicsFrame > 1.0f / PHYSICS_FPS) {
            gameObject->updateEntitiesPrePhysics();
            gameObject->updateEntityPosition();
            gameObject->updateEntitiesPostPhysics();
            deltaPhysicsFrame -= 1.0f / PHYSICS_FPS;
        }
        /*gameObject->updateEntities(deltaTime);
        gameObject->updateEntityPositions(deltaTime);*/
        gameObject->renderEntities(graphics);
        graphics->render();
        graphics->pollEvents();
        frames++;
    }
    return 0;
}