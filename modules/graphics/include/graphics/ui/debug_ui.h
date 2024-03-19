#pragma once

#include "runtime/plugin.h"

#include "ui_manager.h"
#include "util/smooth_queue.h"

namespace phenyl::graphics {
    class ProfileUiPlugin : public runtime::IPlugin {
    private:
        util::SmoothQueue<double, 30> graphicsQueue;
        util::SmoothQueue<double, 30> physicsQueue;
        util::SmoothQueue<double, 30> frameQueue;
        util::SmoothQueue<float, 30> deltaTimeQueue;
    public:
        std::string_view getName() const noexcept override;

        void init(runtime::PhenylRuntime &runtime) override;
        void update(runtime::PhenylRuntime &runtime, double deltaTime) override;
        void render(runtime::PhenylRuntime &runtime) override;
    };
}
