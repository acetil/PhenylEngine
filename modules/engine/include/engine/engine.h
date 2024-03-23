#pragma once

#include <memory>

#include "component/component.h"
#include "component/forward.h"
#include "graphics/graphics.h"
#include "graphics/phenyl_graphics.h"
#include "input/game_input.h"
#include "application.h"

namespace phenyl::engine {
    namespace detail {
        class Engine;
    }

    class PhenylEngine {
    private:
        std::unique_ptr<detail::Engine> internal;

        void exec (ApplicationBase* app);

    public:
        PhenylEngine ();
        ~PhenylEngine();

        template <std::derived_from<ApplicationBase> T, typename ...Args>
        void run (Args&&... args) {
            std::unique_ptr<ApplicationBase> app = std::make_unique<T>(std::forward<Args>(args)...);

            exec(app.get());
            app = nullptr;
            ShutdownLogging();
        }
    };
}