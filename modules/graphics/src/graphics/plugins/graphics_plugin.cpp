#include "graphics/plugins/graphics_plugin.h"

#include "core/input/game_input.h"
#include "core/plugins/input_plugin.h"
#include "core/runtime.h"
#include "graphics/camera_2d.h"
#include "graphics/camera_3d.h"
#include "mesh_manager.h"
#include "texture_manager.h"

using namespace phenyl::graphics;

GraphicsPlugin::GraphicsPlugin () = default;
GraphicsPlugin::~GraphicsPlugin () = default;

std::string_view GraphicsPlugin::getName () const noexcept {
    return "GraphicsPlugin";
}

void GraphicsPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addPlugin<core::InputPlugin>();

    auto& renderer = runtime.resource<Renderer>();
    runtime.addResource(&renderer.getViewport());

    runtime.addResource<Camera2D>(renderer.getViewport().getResolution());
    runtime.addResource<Camera3D>(renderer.getViewport().getResolution());

    renderer.getViewport().addUpdateHandler(&runtime.resource<Camera2D>());
    renderer.getViewport().addUpdateHandler(&runtime.resource<Camera3D>());

    renderer.loadDefaultShaders();
    m_textureManager = std::make_unique<TextureManager>(renderer);
    m_textureManager->selfRegister();

    auto& input = runtime.resource<core::GameInput>();
    renderer.getViewport().addInputDevices(input);

    runtime.addSystem<core::PostInit>("Graphics::PostInit", this, &GraphicsPlugin::postInit);
}

void GraphicsPlugin::postInit (core::PhenylRuntime& runtime) {}
