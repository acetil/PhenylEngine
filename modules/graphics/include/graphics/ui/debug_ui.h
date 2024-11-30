#pragma once

#include "core/plugin.h"

#include "ui_manager.h"
#include "util/smooth_queue.h"

namespace phenyl::graphics {
    class ProfileUiPlugin : public core::IPlugin {
    private:
        util::SmoothQueue<double, 30> graphicsQueue;
        util::SmoothQueue<double, 30> physicsQueue;
        util::SmoothQueue<double, 30> frameQueue;
        util::SmoothQueue<float, 30> deltaTimeQueue;
    public:
        std::string_view getName () const noexcept override;

        void init (core::PhenylRuntime& runtime) override;
        void update (core::PhenylRuntime& runtime);
        void render (core::PhenylRuntime& runtime);
    };
}
