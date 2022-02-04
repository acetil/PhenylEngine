#include <exception>

#include "engine/engine.h"
#include "engine/game_init.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"

#include "logging/logging.h"
#include "game_object.h"

using namespace engine;

class engine::detail::Engine {
private:
    //graphics::detail::Graphics::SharedPtr graphics;
    game::detail::GameObject::SharedPtr gameObj;
    graphics::PhenylGraphicsHolder graphicsHolder;
public:
    Engine ();
    ~Engine();

    [[nodiscard]] game::detail::GameObject::SharedPtr getGameObject () const;
    [[nodiscard]] graphics::PhenylGraphics getGraphics () const;
};

engine::PhenylEngine::PhenylEngine () {
    internal = std::make_unique<engine::detail::Engine>();
}

engine::PhenylEngine::~PhenylEngine () = default;

game::detail::GameObject::SharedPtr engine::PhenylEngine::getGame () {
    return internal->getGameObject();
}

graphics::PhenylGraphics PhenylEngine::getGraphics () {
    return internal->getGraphics();
}

engine::detail::Engine::Engine () {
    /*GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        throw std::runtime_error("Window creation failed!");
    }

    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        throw std::runtime_error("Graphics init failed!");
    }

    logger::log(LEVEL_INFO, "MAIN", "Successfully initialised graphics");*/

    gameObj = game::initGame(graphicsHolder.getGraphics());
}

engine::detail::Engine::~Engine () {
    logger::log(LEVEL_INFO, "MAIN", "Shutting down!");
    //graphics::destroyGraphics(graphics);
}

game::detail::GameObject::SharedPtr detail::Engine::getGameObject () const {
    return gameObj;
}

graphics::PhenylGraphics detail::Engine::getGraphics () const {
    return graphicsHolder.getGraphics();
}
