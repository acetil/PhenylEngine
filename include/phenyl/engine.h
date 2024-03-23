#pragma once

#include "application.h"

#include "graphics/camera.h"

namespace phenyl {
    namespace engine {
        class Engine;
    }

    using Camera = graphics::Camera;

    class PhenylEngine {
    private:
        std::unique_ptr<engine::Engine> internal;

        void exec (engine::ApplicationBase* app);

    public:
        PhenylEngine ();
        ~PhenylEngine();

        template <std::derived_from<engine::ApplicationBase> T, typename ...Args>
        void run (Args&&... args) {
            std::unique_ptr<engine::ApplicationBase> app = std::make_unique<T>(std::forward<Args>(args)...);

            exec(app.get());
            app = nullptr;
            ShutdownLogging();
        }
    };
}