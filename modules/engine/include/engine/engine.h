#pragma once

#include <memory>

#include "component/component.h"
#include "component/forward.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "physics/phenyl_physics.h"
#include "game_camera.h"
#include "input/game_input.h"
#include "application.h"

namespace phenyl::engine {

    namespace detail {
        class Engine;
    }

    class PhenylEngine {
    private:
        std::unique_ptr<detail::Engine> internal;

        /*void gameloop (Application* app);
        void update (Application* app, double deltaTime);
        void fixedUpdate (Application* app, double deltaTime);
        void render (Application* app);*/
        void exec (Application* app);

    public:
        PhenylEngine ();
        ~PhenylEngine();
        //game::PhenylGame getGame ();

        graphics::PhenylGraphics getGraphics ();

        component::EntityComponentManager& getComponentManager ();

        component::EntitySerializer& getEntitySerializer ();

        physics::PhenylPhysics getPhysics ();

        game::GameCamera& getCamera ();
        game::GameInput& getInput ();

        void dumpLevel (std::ostream& path);

        void updateEntityPosition (float deltaTime);
        void debugRender ();
        void setDebugRender (bool doRender);
        void setProfileRender (bool doRender);

        template <std::derived_from<Application> T, typename ...Args>
        void run (Args&&... args) {
            std::unique_ptr<Application> app = std::make_unique<T>(std::forward<Args>(args)...);
            app->engine = this;

            exec(app.get());
        }
    };
}