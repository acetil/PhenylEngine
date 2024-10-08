#include "common/plugins/core_plugin_2d.h"
#include "runtime/runtime.h"

#include "graphics/renderer.h"
#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/entity_layer.h"

#include "graphics/components/2d/sprite.h"
#include "graphics/plugins/sprite_2d_plugin.h"

using namespace phenyl::graphics;

std::string_view Sprite2DPlugin::getName () const noexcept {
    return "Sprite2DPlugin";
}

void Sprite2DPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();
    runtime.addPlugin<common::Core2DPlugin>();

    runtime.addComponent<Sprite2D>("Sprite2D");

    auto& renderer = runtime.resource<Renderer>();
    entityLayer = &renderer.addLayer<EntityRenderLayer>();

    entityLayer->addSystems(runtime);
}
