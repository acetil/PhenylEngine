#include "graphics/graphics.h"
#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/debug_layer.h"

#include "graphics/components/2d/sprite_serialization.h"

using namespace phenyl::graphics;

std::string_view GraphicsPlugin::getName () const noexcept {
    return "GraphicsPlugin";
}

void GraphicsPlugin::init (runtime::PhenylRuntime& runtime) {
    graphics = &runtime.resource<detail::Graphics>();

    runtime.addResource<graphics::Renderer>(graphics->getRenderer());
    runtime.addResource(&graphics->getCamera());

    auto& renderer = runtime.resource<Renderer>();
    debugLayer = &renderer.addLayer<DebugLayer>();
}

void GraphicsPlugin::render (runtime::PhenylRuntime& runtime) {
    auto& renderer = runtime.resource<Renderer>();
    const auto& camera = runtime.resource<const Camera>();

    PHENYL_DASSERT(debugLayer);
    debugLayer->bufferData(camera, renderer.getViewport().getResolution());
}
