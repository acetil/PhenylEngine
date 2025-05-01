#pragma once

#include "core/plugin.h"

#include "ui_manager.h"
#include "util/smooth_queue.h"

namespace phenyl::graphics {
    class ProfileUiPlugin : public core::IPlugin {
    public:
        std::string_view getName () const noexcept override;

        void init (core::PhenylRuntime& runtime) override;
        void update (core::PhenylRuntime& runtime);
        void render (core::PhenylRuntime& runtime);

    private:
        util::SmoothQueue<double, 30> m_graphicsQueue;
        util::SmoothQueue<double, 30> m_physicsQueue;
        util::SmoothQueue<double, 30> m_frameQueue;
        util::SmoothQueue<float, 30> m_deltaTimeQueue;
    };
}
