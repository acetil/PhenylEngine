#include "graphics/graphics.h"
#include "graphics/graphics_plugin.h"

#include "graphics/components/2d/sprite_serialization.h"

using namespace phenyl::graphics;

std::string_view GraphicsPlugin::getName () const noexcept {
    return "GraphicsPlugin";
}

void GraphicsPlugin::init (runtime::PhenylRuntime& runtime) {
    graphics = &runtime.resource<detail::Graphics>();

    runtime.addResource<graphics::Renderer>(graphics->getRenderer());
    runtime.addResource(&graphics->getCamera());

    runtime.addComponent<Sprite2D>();

    graphics->addEntityLayer(&runtime.manager()); // TODO: unhackify
    graphics->setupWindowCallbacks();
}
