#include "runtime/runtime.h"

#include "graphics/components/2d/sprite_serialization.h"
#include "graphics/graphics.h"
#include "graphics/plugins/sprite_2d_plugin.h"
#include "graphics/plugins/graphics_plugin.h"
#include "common/plugins/core_plugin_2d.h"
#include "graphics/renderlayer/entity_layer.h"

using namespace phenyl::graphics;

std::string_view Sprite2DPlugin::getName () const noexcept {
    return "Sprite2DPlugin";
}

void Sprite2DPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();
    runtime.addPlugin<common::Core2DPlugin>();

    runtime.addComponent<Sprite2D>();

    auto& graphics = runtime.resource<detail::Graphics>();
    //graphics.addEntityLayer(&runtime.manager()); // TODO: unhackify

    auto& renderer = runtime.resource<Renderer>();
    entityLayer = &renderer.addLayer<EntityRenderLayer>();
}

void Sprite2DPlugin::render (phenyl::runtime::PhenylRuntime& runtime) {
    const auto& camera = runtime.resource<const Camera>();

    PHENYL_DASSERT(entityLayer);
    entityLayer->preRender(runtime.manager(), camera);
}
