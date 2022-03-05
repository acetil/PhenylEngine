#include <stdlib.h>

//#include "graphics/graphics_headers.h"
//#include "graphics/graphics_init.h"

#include "game/gameloop.h"

#include "logging/logging.h"

//#include "util/format.h"

//#include "util/data.h"

#include "component/component.h"
#include "util/map.h"

int main (int argv, char* argc[]) {
    // TODO: move to exceptions

    //util::testData();

    logger::initLogger();

    logger::log(LEVEL_DEBUG, "MAIN", "Started game!");
    /*logger::log(LEVEL_DEBUG, "MAIN", util::format("Test format: {} {} {1}", 4, 3.8, "abc"));
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    graphics::Graphics::SharedPtr graphics;
    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        return GRAPHICS_INIT_FAILURE;
    }
    logger::log(LEVEL_INFO, "MAIN", "Successfully initialised graphics");
    game::gameloop(graphics);
    logger::log(LEVEL_INFO, "MAIN", "Shutting down!");
    graphics::destroyGraphics(graphics);*/

    engine::PhenylEngine engine;

    game::gameloop(engine);

    auto compManager = component::ComponentManagerNew::NewSharedPtr(256);

    compManager->addComponentType<int>();
    compManager->addComponentType<glm::vec2>();

    auto eId = compManager->createEntity();
    compManager->addComponent<int>(eId, 4);
    compManager->addComponent<glm::vec2>(eId, glm::vec2{3, 3});

    auto view = compManager->getConstrainedView<int, glm::vec2>().orThrow();

    component::view::ConstrainedEntityView<int, glm::vec2> eView = view.getEntityView(eId).orThrow();
    logger::log(LEVEL_DEBUG, "MAIN", util::format("Comp: {}", eView.get<int>()));

    auto view2 = view.constrain<glm::vec2>();
    auto eView2 = view2.getEntityView(eId).orThrow();

    logger::log(LEVEL_DEBUG, "MAIN", util::format("Comp2: <{}, {}>", eView2.get<glm::vec2>()[0], eView.get<glm::vec2>()[1]));
    //auto view3 = view.constrain<float>();

    static_assert(std::forward_iterator<util::MapIterator<std::string, int>>);
    static_assert(std::random_access_iterator<component::view::detail::EntityViewIterator>);

    return EXIT_SUCCESS;
}