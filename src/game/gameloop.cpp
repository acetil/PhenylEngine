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

#define TARGET_FPS 60
#define PHYSICS_FPS 60
using namespace game; 

int game::gameloop (graphics::Graphics* graphics) {
    GameObject* gameObject = initGame(graphics);
    gameObject->createNewEntityInstance("test_entity", 0.5, 0.5);
    //gameObject->createNewEntityInstance("bullet", 0.3, 0.3);
    logging::log(LEVEL_INFO, "Created player");
    KeyboardInput* keyInput = getKeyboardInput(graphics);
    setupMovementKeys(keyInput, gameObject->getEventBus());

    // TODO: remove
    graphics::FontManager manager;
    manager.addFace("noto-serif", "/usr/share/fonts/noto/NotoSerif-Regular.ttf");
    auto& face = manager.getFace("noto-serif");
    face.setFontSize(72);
    face.setGlyphs({graphics::AsciiGlyphRange});
    graphics::GlyphAtlas atlas(face.getGlyphs(), 64);
    atlas.loadAtlas(graphics->getRenderer());
    graphics->addGlyphAtlas(atlas);

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
    while (!graphics->shouldClose()) {
        deltaTime = (float) graphics->getDeltaTime();
        deltaPhysicsFrame += deltaTime;
        timeSinceFpsUpdate += deltaTime;
        keyInput->handleKeyPresses();
        while (deltaPhysicsFrame >= 1.0f / PHYSICS_FPS) {
            gameObject->updateEntitiesPrePhysics();
            gameObject->updateEntityPosition();
            gameObject->updateEntitiesPostPhysics();
            deltaPhysicsFrame -= 1.0f / PHYSICS_FPS;
            //fpsFrames++;
        }
        if (timeSinceFpsUpdate >= 1.0f) {
            logging::log(LEVEL_DEBUG, "Done {} frames in {} second(s), with an average fps of {}", frames,
                          timeSinceFpsUpdate, (float)frames / timeSinceFpsUpdate);
            timeSinceFpsUpdate = 0.0f;
            frames = 0;
            //fpsFrames = 0;
        }
        /*gameObject->updateEntities(deltaTime);
        gameObject->updateEntityPositions(deltaTime);*/
        //gameObject->renderEntities(graphics);
        graphics->render();
        graphics->pollEvents();
        frames++;
        graphics->sync(TARGET_FPS);
    }
    return 0;
}