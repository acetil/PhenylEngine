#pragma once

#include <iosfwd>

#include "component/forward.h"
#include "forward.h"
#include "graphics/ui/forward.h"

#include "component/component.h"
#include "component/component_serializer.h"

namespace phenyl::engine {
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
        component::EntitySerializer& serializer ();
    protected:
        component::ComponentManager& componentManager ();
        game::GameCamera& camera ();
        game::GameInput& input ();
        graphics::UIManager& uiManager ();
        void setTargetFPS (double fps);
        void setFixedTimeScale (double newTimeScale);
        void setDebugRender (bool doRender); // TODO: remove?
        void setProfileRender (bool doRender); // TODO: remove?
        void dumpLevel (std::ostream& file);
    public:
        virtual ~Application() = default;

        virtual void init () = 0;
        virtual void update (double deltaTime) = 0;
        virtual void fixedUpdate (float deltaTime) = 0;
        virtual void shutdown () {

        }

        void pause ();
        void resume ();

        template <phenyl::common::CustomSerializable T>
        void addComponent () {
            componentManager().template addComponent<T>();
            serializer().template addSerializer<T>();
        };

        template <typename T>
        void addUnserializedComponent () {
            componentManager().template addComponent<T>();
        }
    };
}