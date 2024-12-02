#include "core/input/game_input.h"
#include "core/runtime.h"

#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/debug_layer.h"

#include "texture_manager.h"
#include "core/plugins/input_plugin.h"

using namespace phenyl::graphics;

struct GraphicsData : public phenyl::core::IResource {
    DebugLayer* layer;

    [[nodiscard]] std::string_view getName() const noexcept override {
        return "GraphicsData";
    }

    GraphicsData (DebugLayer* layer) : layer{layer} {}
};

static void DebugRenderSystem (const phenyl::core::Resources<const Viewport, const Camera, GraphicsData>& resources) {
    auto& [viewport, camera, data] = resources;
    data.layer->bufferData(camera, viewport.getResolution());
}



GraphicsPlugin::GraphicsPlugin () = default;
GraphicsPlugin::~GraphicsPlugin () = default;

std::string_view GraphicsPlugin::getName () const noexcept {
    return "GraphicsPlugin";
}

void GraphicsPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addPlugin<core::InputPlugin>();

    auto& renderer = runtime.resource<Renderer>();
    runtime.addResource(&renderer.getViewport());

    runtime.addResource<Camera>(renderer.getViewport().getResolution());

    auto& camera = runtime.resource<Camera>();
    renderer.getViewport().addUpdateHandler(&camera);

    renderer.loadDefaultShaders();
    textureManager = std::make_unique<TextureManager>(renderer);
    textureManager->selfRegister();

    auto* debugLayer = &renderer.addLayer<DebugLayer>();
    runtime.addResource<GraphicsData>(debugLayer);

    auto& input = runtime.resource<core::GameInput>();
    renderer.getViewport().addInputDevices(input);

    runtime.addSystem<core::PostInit>("Graphics::PostInit", this, &GraphicsPlugin::postInit);
    runtime.addSystem<core::Render>("Graphics::DebugRender", DebugRenderSystem);
}

void GraphicsPlugin::postInit (core::PhenylRuntime& runtime) {

}
