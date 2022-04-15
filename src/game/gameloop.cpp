#include "gameloop.h"
#include "engine/game_init.h"
#include "engine/game_object.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"
#include "engine/key_input.h"
#include "engine/key_defaults.h"
#include "engine/map/map_reader.h"

#include "graphics/graphics.h"
#include "graphics/renderlayer/map_layer.h"

#include "graphics/font/font_manager.h"
#include "graphics/font/glyph_atlas.h"

#include "graphics/ui/debug_ui.h"

#include "util/profiler.h"

#define TARGET_FPS 60
#define PHYSICS_FPS 60
using namespace game; 

int game::gameloop (engine::PhenylEngine& engine) {
    //GameObject::SharedPtr gameObject = initGame(graphics);
    auto graphics = engine.getGraphics();
    auto gameObject = engine.getGame();

    auto& uiManager = graphics.getUIManager();

    //gameObject->createNewEntityInstance("bullet", 0.3, 0.3);
    logging::log(LEVEL_INFO, "Created player");
    KeyboardInput::SharedPtr keyInput = getKeyboardInput(engine.getGraphics());
    setupMovementKeys(keyInput, engine.getEventBus());

    float deltaTime;
    float deltaPhysicsFrame = 0.0f;
    //float timeSinceFpsUpdate = 0.0f;
    //int frames = 0;
    //int fpsFrames = 0;
    //Map::SharedPtr map = readMap("resources/maps/testmap.acmp", gameObject);
    //gameObject->createNewEntityInstance("test_entity", 0.5, 0.5);
    Map::SharedPtr map = readMap("resources/maps/testmap.json", gameObject);
    //map->setAtlas(graphics->getTextureAtlas("sprite").value());
    gameObject.loadMap(map);
    //gameObject->createNewEntityInstance("test_entity", 0.5, 0.5);
    //map->initGraphicsData(graphics, "default");
    //std::dynamic_pointer_cast<graphics::MapRenderLayer>(graphics->getRenderLayer()->getRenderLayer("map_layer").value())->attachMap(map); // TODO: make easier (event?)
    logging::log(LEVEL_DEBUG, "Starting loop");

    while (!graphics.shouldClose()) {
        util::startProfileFrame();
        deltaTime = (float) graphics.getDeltaTime();
        deltaPhysicsFrame += deltaTime;
        //timeSinceFpsUpdate += deltaTime;
        keyInput->handleKeyPresses();
        while (deltaPhysicsFrame >= 1.0f / PHYSICS_FPS) {
            util::startProfile("physics");
            gameObject.updateEntitiesPrePhysics();
            gameObject.updateEntityPosition();
            gameObject.updateEntitiesPostPhysics();
            deltaPhysicsFrame -= 1.0f / PHYSICS_FPS;
            util::endProfile();

            gameObject.updateCamera(graphics.getCamera());
            //fpsFrames++;
        }
        /*if (timeSinceFpsUpdate >= 1.0f) {
            logging::log(LEVEL_DEBUG, "Done {} frames in {} second(s), with an average fps of {}", frames,
                          timeSinceFpsUpdate, (float)frames / timeSinceFpsUpdate);
            timeSinceFpsUpdate = 0.0f;
            frames = 0;
            //fpsFrames = 0;
        }*/

        util::startProfile("graphics");

        graphics::renderDebugUi(gameObject, uiManager, deltaTime);

        uiManager.renderUI();
        graphics.render();

        util::endProfile();

        graphics.pollEvents();
        //frames++;
        util::endProfileFrame();

        graphics.sync(TARGET_FPS);
    }
    return 0;
}