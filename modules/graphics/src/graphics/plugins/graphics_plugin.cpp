#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/debug_layer.h"

#include "texture_manager.h"
#include "graphics/components/2d/sprite_serialization.h"

using namespace phenyl::graphics;

struct GraphicsData : public phenyl::runtime::IResource {
    DebugLayer* layer;

    [[nodiscard]] std::string_view getName() const noexcept override {
        return "GraphicsData";
    }

    GraphicsData (DebugLayer* layer) : layer{layer} {}
};

static void DebugRenderSystem (const phenyl::runtime::Resources<const Viewport, const Camera, GraphicsData>& resources) {
    auto& [viewport, camera, data] = resources;
    data.layer->bufferData(camera, viewport.getResolution());
}

GraphicsPlugin::GraphicsPlugin () = default;
GraphicsPlugin::~GraphicsPlugin () = default;

std::string_view GraphicsPlugin::getName () const noexcept {
    return "GraphicsPlugin";
}

void GraphicsPlugin::init (runtime::PhenylRuntime& runtime) {
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

    runtime.addSystem<runtime::Render>(DebugRenderSystem);
}
