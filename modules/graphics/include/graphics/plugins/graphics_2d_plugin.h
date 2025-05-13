#pragma once

#include "core/plugin.h"
#include "core/runtime.h"
#include "graphics_plugin.h"
#include "particle_plugin.h"
#include "sprite_2d_plugin.h"
#include "ui_plugin.h"

namespace phenyl::graphics {
class Graphics2DPlugin : public core::IInitPlugin {
public:
    std::string_view getName () const noexcept override {
        return "Graphics2DPlugin";
    }

    void init (core::PhenylRuntime& runtime) override {
        runtime.addPlugin<GraphicsPlugin>();
        runtime.addPlugin<Particle2DPlugin>();
        runtime.addPlugin<Sprite2DPlugin>();
        runtime.addPlugin<UIPlugin>();
    }
};
} // namespace phenyl::graphics
