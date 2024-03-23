#pragma once

#include "runtime/init_plugin.h"
#include "runtime/runtime.h"

#include "graphics_plugin.h"
#include "particle_plugin.h"
#include "sprite_2d_plugin.h"
#include "ui_plugin.h"

namespace phenyl::graphics {
    class Graphics2DPlugin : public runtime::IInitPlugin {
    public:
        std::string_view getName () const noexcept override {
            return "Graphics2DPlugin";
        }

        void init (runtime::PhenylRuntime& runtime) override {
            runtime.addPlugin<GraphicsPlugin>();
            runtime.addPlugin<Particle2DPlugin>();
            runtime.addPlugin<Sprite2DPlugin>();
            runtime.addPlugin<UIPlugin>();
        }
    };
}