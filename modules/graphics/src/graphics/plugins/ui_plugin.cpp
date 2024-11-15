#include "graphics/plugins/ui_plugin.h"

#include "common/input/game_input.h"
#include "common/plugins/input_plugin.h"
#include "graphics/plugins/graphics_plugin.h"

using namespace phenyl::graphics;

static void UIUpdateSystem (const phenyl::runtime::Resources<UIManager>& resources) {
    resources.get<UIManager>().updateUI();
}

static void UIRenderSystem (const phenyl::runtime::Resources<UIManager, Canvas>& resources) {
    auto& [manager, canvas] = resources;
    manager.renderUI(canvas);
}

std::string_view UIPlugin::getName () const noexcept {
    return "UIPlugin";
}

void UIPlugin::init (phenyl::runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();
    runtime.addPlugin<common::InputPlugin>();

    auto& renderer = runtime.resource<Renderer>();
    auto& input = runtime.resource<common::GameInput>();
    manager = std::make_unique<UIManager>(input);

    runtime.addResource<Canvas>(renderer);
    runtime.addResource(manager.get());

    runtime.addSystem<runtime::FrameBegin>("UIManager::Update", UIUpdateSystem);
    runtime.addSystem<runtime::Render>("UIManager::Render", UIRenderSystem);
}
