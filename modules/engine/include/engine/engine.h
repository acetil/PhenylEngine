#pragma once

#include <memory>

#include "component/component.h"
#include "game_object.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "engine/phenyl_game.h"

namespace engine {

    namespace detail {
        class Engine;
    }

    class PhenylEngine {
    private:
        std::unique_ptr<detail::Engine> internal;
    public:
        PhenylEngine ();
        ~PhenylEngine();

        game::detail::GameObject::SharedPtr getGameTemp ();

        game::PhenylGame getGame ();

        graphics::PhenylGraphics getGraphics ();

        event::EventBus::SharedPtr getEventBus ();

        component::EntityComponentManager::SharedPtr getComponentManager ();
    };
}