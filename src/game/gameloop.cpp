#include "gameloop.h"
#include "engine/game_init.h"
#include "engine/game_object.h"
#include "graphics/graphics_headers.h"
#include "logging/logging.h"
#include "graphics/shaders/shaders.h"
#include "engine/map/map_reader.h"

#include "graphics/graphics.h"
#include "graphics/renderlayer/map_layer.h"
#include "graphics/ui/nodes/ui_button.h"
#include "graphics/ui/nodes/ui_flexbox.h"
#include "graphics/ui/nodes/ui_label.h"
#include "graphics/ui/components/ui_button.h"
#include "graphics/ui/components/ui_flexbox.h"
#include "graphics/ui/components/ui_label.h"

#include "graphics/font/font_manager.h"
#include "graphics/font/glyph_atlas.h"

#include "graphics/ui/debug_ui.h"

#include "util/profiler.h"
#include "default_input.h"

#define TARGET_FPS 60
#define PHYSICS_FPS 60
using namespace game; 

int game::gameloop (engine::PhenylEngine& engine) {
    //GameObject::SharedPtr gameObject = initGame(graphics);
    std::vector<graphics::ui::UILabel> extraLabels;
    auto graphics = engine.getGraphics();
    auto gameObject = engine.getGame();

    auto& uiManager = graphics.getUIManager();

    //gameObject->createNewEntityInstance("bullet", 0.3, 0.3);
    //logging::log(LEVEL_INFO, "Created player");
    gameObject.addEntityType("test_entity", "resources/entity_types/test_entity.json");
    gameObject.addEntityType("wall_entity", "resources/entity_types/wall_entity.json");
    gameObject.addEntityType("bullet_entity", "resources/entity_types/bullet_entity.json");

    game::GameInput& gameInput = gameObject.getGameInput();
    setupDefaultInput(gameInput, gameObject.getEventBus());
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

    auto buttonC = graphics::ui::UIButton("button");
    auto buttonC2 = graphics::ui::UIButton("button2");
    //auto buttonC3 = graphics::ui::UIButton("button3");
    auto flexBoxC = graphics::ui::UIFlexbox("flex_box");

    auto label = graphics::ui::UILabel("label");
    label.text = "Hello World!";
    logging::log(LEVEL_DEBUG, "Component label text: {}", (std::string)label.text);

    flexBoxC.add(buttonC.detach());
    flexBoxC.add(buttonC2.detach());
    flexBoxC.add(label);
    //flexBoxC.insert(buttonC3.detach());

    auto button4 = graphics::ui::UIButton("button");
    auto button5 = graphics::ui::UIButton("button");

    int numPresses = 0;
    bool isButtonDown = false;
    bool isButtonDown2 = false;

    uiManager.addUIComp(flexBoxC.detach(), {0, 100});
    uiManager.addUIComp(button4, {500, 300});
    uiManager.addUIComp(button5, {500, 385});
    uiManager.addTheme("resources/themes/default_theme.json");
    uiManager.addTheme("resources/themes/alt_theme.json");
    uiManager.setCurrentTheme("default_theme");
    //uiManager.addUINode(labelNode, {500, 300});


    while (!graphics.shouldClose()) {
        util::startProfileFrame();
        flexBoxC.setAxes(graphics::ui::Axis::DOWN, graphics::ui::Axis::RIGHT);
        deltaTime = (float) graphics.getDeltaTime();
        deltaPhysicsFrame += deltaTime;
        //timeSinceFpsUpdate += deltaTime;
        //keyInput->handleKeyPresses();
        graphics.updateUI();
        gameInput.poll();
        while (deltaPhysicsFrame >= 1.0f / PHYSICS_FPS) {
            util::startProfile("physics");
            gameObject.updateEntitiesPrePhysics();
            //gameObject.updateEntityPosition();
            engine.updateEntityPosition();
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
        engine.debugRender();

        //uiManager.renderRect({100, 200}, {50, 200}, {0.0f, 0.0f, 1.0f, 1.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}, 8, 2);

        if (button4 && !isButtonDown) {
            //logging::log(LEVEL_INFO, "Button down, changing text!");
            isButtonDown = true;
            numPresses++;
            label.text = "Pressed " + std::to_string(numPresses) + " times!";
            auto newLabel = graphics::ui::UILabel("label");
            newLabel.text = "Label " + std::to_string(extraLabels.size());
            flexBoxC.add(newLabel);
            extraLabels.emplace_back(std::move(newLabel));
            //button4.detachNode();
            //button4 = graphics::ui::UIButton("button");
        } else if (!button4 && isButtonDown) {
            isButtonDown = false;
        }

        if (button5 && !isButtonDown2) {
            isButtonDown2 = true;

            if (!extraLabels.empty()) {
                //extraLabels.at(extraLabels.size() - 1).destroy();
                extraLabels.pop_back();
            }
        } else if (!button5 && isButtonDown2){
            isButtonDown2 = false;
        }

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