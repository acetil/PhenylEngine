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

    public:
        PhenylEngine (const logging::LoggingProperties& = {});
        ~PhenylEngine();

        template <std::derived_from<engine::ApplicationBase> T, typename ...Args>
        void run (Args&&... args) {
            run(std::make_unique<T>(std::forward<Args>(args)...));
        }

        void run (std::unique_ptr<engine::ApplicationBase> app);
    };
}