#include "gameloop.h"
#include "game_init.h"
#include "game_object.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"
#include "key_input.h"
#include "key_defaults.h"
#include "map/map_reader.h"

#include "graphics/graphics.h"
#include "graphics/renderlayer/map_layer.h"

#include "graphics/font/font_manager.h"
#include "graphics/font/glyph_atlas.h"

#include "util/profiler.h"

#define TARGET_FPS 60
#define PHYSICS_FPS 60
using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    GameObject* gameObject = initGame(graphics);

    auto& uiManager = graphics->getUIManager();

    gameObject->createNewEntityInstance("test_entity", 0.5, 0.5);
    //gameObject->createNewEntityInstance("bullet", 0.3, 0.3);
    logging::log(LEVEL_INFO, "Created player");
    KeyboardInput* keyInput = getKeyboardInput(graphics);
    setupMovementKeys(keyInput, gameObject->getEventBus());

    float deltaTime;
    float deltaPhysicsFrame = 0.0f;
    float timeSinceFpsUpdate = 0.0f;
    int frames = 0;
    //int fpsFrames = 0;
    Map* map = readMap("resources/maps/testmap.acmp", gameObject);
    map->setAtlas(graphics->getTextureAtlas("sprite").value());
    //map->initGraphicsData(graphics, "default");
    ((graphics::MapRenderLayer*)graphics->getRenderLayer()->getRenderLayer("map_layer").value())->attachMap(map); // TODO: make easier (event?)
    logging::log(LEVEL_DEBUG, "Starting loop");

    double avgGraphicsTime = 0;
    double avgPhysicsTime = 0;
    double avgFrameTime = 0;

    double totGraphicsTime = 0;
    double totPhysicsTime = 0;
    double totFrameTime = 0;

    int smoothFrames = 0;

    while (!graphics->shouldClose()) {
        util::startProfileFrame();
        deltaTime = (float) graphics->getDeltaTime();
        deltaPhysicsFrame += deltaTime;
        timeSinceFpsUpdate += deltaTime;
        keyInput->handleKeyPresses();
        while (deltaPhysicsFrame >= 1.0f / PHYSICS_FPS) {
            util::startProfile("physics");
            gameObject->updateEntitiesPrePhysics();
            gameObject->updateEntityPosition();
            gameObject->updateEntitiesPostPhysics();
            deltaPhysicsFrame -= 1.0f / PHYSICS_FPS;
            util::endProfile();
            //fpsFrames++;
        }
        if (timeSinceFpsUpdate >= 1.0f) {
            logging::log(LEVEL_DEBUG, "Done {} frames in {} second(s), with an average fps of {}", frames,
                          timeSinceFpsUpdate, (float)frames / timeSinceFpsUpdate);
            timeSinceFpsUpdate = 0.0f;
            frames = 0;
            //fpsFrames = 0;
        }

        if (smoothFrames % 30 == 0) {
            avgFrameTime = totFrameTime / 30;
            avgGraphicsTime = totGraphicsTime / 30;
            avgPhysicsTime = totPhysicsTime / 30;

            totFrameTime = 0;
            totGraphicsTime = 0;
            totPhysicsTime = 0;
        }

        totFrameTime += util::getProfileFrameTime();
        totGraphicsTime += util::getProfileTime("graphics");
        totPhysicsTime += util::getProfileTime("physics");

        smoothFrames++;

        util::startProfile("graphics");
        //uiManager.renderText("noto-serif", "Hello World!", 28, 100, 50);

        uiManager.renderText("noto-serif", "physics: " + std::to_string(avgPhysicsTime * 1000) + "ms", 11, 20, 20);
        uiManager.renderText("noto-serif", "graphics: " + std::to_string(avgGraphicsTime * 1000) + "ms", 11, 20, 45);
        uiManager.renderText("noto-serif", "frame time: " + std::to_string(avgFrameTime * 1000) + "ms", 11, 20, 70);
        graphics->render();

        util::endProfile();

        graphics->pollEvents();
        frames++;
        util::endProfileFrame();

        graphics->sync(TARGET_FPS);
    }
    return 0;
}