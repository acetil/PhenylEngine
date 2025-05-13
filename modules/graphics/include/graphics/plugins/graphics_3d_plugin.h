#pragma once

#include "core/plugin.h"
#include "core/runtime.h"
#include "graphics_plugin.h"
#include "mesh_3d_plugin.h"
#include "ui_plugin.h"

namespace phenyl::graphics {
class Graphics3DPlugin : public core::IInitPlugin {
public:
    std::string_view getName () const noexcept override {
        return "Graphics3DPlugin";
    }

    void init (core::PhenylRuntime& runtime) override {
        runtime.addPlugin<GraphicsPlugin>();
        runtime.addPlugin<Mesh3DPlugin>();
        runtime.addPlugin<UIPlugin>();
    }
};
} // namespace phenyl::graphics
