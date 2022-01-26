#include <exception>

#include "engine/engine.h"
#include "engine/game_init.h"

#include "graphics/graphics_init.h"
#include "graphics/graphics_headers.h"

#include "logging/logging.h"

using namespace engine;

class engine::detail::Engine {
private:
    graphics::Graphics::SharedPtr graphics;
    game::GameObject::SharedPtr gameObj;
public:
    Engine ();
    ~Engine();

    graphics::Graphics::SharedPtr getGraphics () const;
    game::GameObject::SharedPtr getGameObject () const;
};

engine::PhenylEngine::PhenylEngine () {
    internal = std::make_unique<engine::detail::Engine>();
}

engine::PhenylEngine::~PhenylEngine () = default;

graphics::Graphics::SharedPtr engine::PhenylEngine::getGraphics () {
    return internal->getGraphics();
}

game::GameObject::SharedPtr engine::PhenylEngine::getGame () {
    return internal->getGameObject();
}

engine::detail::Engine::Engine () {
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Window init failure, stopping!");
        throw std::runtime_error("Window creation failed!");
    }

    if (graphics::initGraphics(window, graphics) != GRAPHICS_INIT_SUCCESS) {
        logger::log(LEVEL_FATAL, "MAIN", "Graphics init failure, stopping!");
        throw std::runtime_error("Graphics init failed!");
    }

    logger::log(LEVEL_INFO, "MAIN", "Successfully initialised graphics");

    gameObj = game::initGame(graphics);
}

engine::detail::Engine::~Engine () {
    logger::log(LEVEL_INFO, "MAIN", "Shutting down!");
    graphics::destroyGraphics(graphics);
}

graphics::Graphics::SharedPtr detail::Engine::getGraphics () const {
    return graphics;
}

game::GameObject::SharedPtr detail::Engine::getGameObject () const {
    return gameObj;
}
