#include "graphics/plugins/ui_plugin.h"
#include "graphics/graphics.h"
#include "graphics/plugins/graphics_plugin.h"

using namespace phenyl::graphics;

std::string_view UIPlugin::getName () const noexcept {
    return "UIPlugin";
}

void UIPlugin::init (phenyl::runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();

    auto& graphics = runtime.resource<detail::Graphics>();
    auto& renderer = runtime.resource<Renderer>();

    manager = std::make_unique<UIManager>(&renderer);

    manager->addProxyInputSources(graphics.getProxySources());
    manager->setupInputActions();

    manager->addRenderLayer(graphics, &renderer);

    runtime.addResource(manager.get());
}

void UIPlugin::frameBegin (runtime::PhenylRuntime& runtime) {
    manager->setMousePos(runtime.resource<Renderer>().getMousePos());
    manager->updateUI();
}

void UIPlugin::render (runtime::PhenylRuntime& runtime) {
    manager->renderUI();
}
