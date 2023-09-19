#pragma once

#include "component/forward.h"
#include "forward.h"
#include "graphics/ui/forward.h"
#include "event/forward.h"

namespace engine {
    namespace detail {
        class Engine;
    }
    class Application {
    private:
        PhenylEngine* engine = nullptr;
        double targetFrameTime{1.0 / 60};
        double targetFps{60};
        double fixedTimeScale{1.0};

        friend class PhenylEngine;
        friend class detail::Engine;
    protected:
        component::ComponentManager& componentManager ();
        component::EntitySerializer& serializer ();
        game::GameCamera& camera ();
        game::GameInput& input ();
        std::shared_ptr<event::EventBus> eventBus ();
        graphics::UIManager& uiManager ();
        void setTargetFPS (double fps);
        void setFixedTimeScale (double newTimeScale);
    public:
        virtual ~Application() = default;

        virtual void init () = 0;
        virtual void update (double deltaTime) = 0;
        virtual void fixedUpdate (float deltaTime) = 0;
        virtual void shutdown () {

        }

        void pause ();
        void resume ();
    };
}