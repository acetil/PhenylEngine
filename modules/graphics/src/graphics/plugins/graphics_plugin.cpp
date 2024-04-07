#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/debug_layer.h"

#include "texture_manager.h"
#include "graphics/components/2d/sprite_serialization.h"

using namespace phenyl::graphics;

GraphicsPlugin::GraphicsPlugin () = default;
GraphicsPlugin::~GraphicsPlugin () = default;

std::string_view GraphicsPlugin::getName () const noexcept {
    return "GraphicsPlugin";
}

void GraphicsPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addResource<Camera>();

    auto& renderer = runtime.resource<Renderer>();
    renderer.loadDefaultShaders();
    textureManager = std::make_unique<TextureManager>(renderer);
    textureManager->selfRegister();

    debugLayer = &renderer.addLayer<DebugLayer>();
}

void GraphicsPlugin::render (runtime::PhenylRuntime& runtime) {
    auto& renderer = runtime.resource<Renderer>();
    const auto& camera = runtime.resource<const Camera>();

    PHENYL_DASSERT(debugLayer);
    debugLayer->bufferData(camera, renderer.getViewport().getResolution());
}

void GraphicsPlugin::shutdown (runtime::PhenylRuntime& runtime) {
    textureManager = nullptr;
}
