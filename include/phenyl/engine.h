#pragma once

#include "application.h"
#include "graphics/camera_2d.h"
#include "graphics/camera_3d.h"

namespace phenyl {
namespace engine {
    class Engine;
}

using Camera2D = graphics::Camera2D;
using Camera3D = graphics::Camera3D;

class PhenylEngine {
public:
    PhenylEngine (const logging::LoggingProperties& = {});
    ~PhenylEngine ();

    template <std::derived_from<engine::ApplicationBase> T, typename... Args>
    void run (Args&&... args) {
        run(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void run (std::unique_ptr<engine::ApplicationBase> app);

private:
    std::unique_ptr<engine::Engine> m_internal;
};
} // namespace phenyl
