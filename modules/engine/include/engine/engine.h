#pragma once

#include <memory>

#include "component/component.h"
#include "component/forward.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "physics/phenyl_physics.h"
#include "game_camera.h"
#include "input/game_input.h"

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
        //game::PhenylGame getGame ();

        graphics::PhenylGraphics getGraphics ();

        event::EventBus::SharedPtr getEventBus ();

        component::EntityComponentManager& getComponentManager ();

        component::EntitySerializer& getEntitySerializer ();

        physics::PhenylPhysics getPhysics ();

        game::GameCamera& getCamera ();
        game::GameInput& getInput ();

        void dumpLevel (std::ostream& path);

        void updateEntityPosition (float deltaTime);
        void debugRender ();
    };
}