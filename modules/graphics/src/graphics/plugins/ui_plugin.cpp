#include "graphics/plugins/ui_plugin.h"

#include "core/input/game_input.h"
#include "core/plugins/input_plugin.h"
#include "core/runtime.h"
#include "graphics/plugins/graphics_plugin.h"

using namespace phenyl::graphics;

static void UIUpdateSystem (const phenyl::core::Resources<UIManager>& resources) {
    resources.get<UIManager>().updateUI();
}

static void UIRenderSystem (const phenyl::core::Resources<UIManager, Canvas>& resources) {
    auto& [manager, canvas] = resources;
    manager.renderUI(canvas);
}

std::string_view UIPlugin::getName () const noexcept {
    return "UIPlugin";
}

void UIPlugin::init (phenyl::core::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();
    runtime.addPlugin<core::InputPlugin>();

    auto& renderer = runtime.resource<Renderer>();
    auto& input = runtime.resource<core::GameInput>();
    m_manager = std::make_unique<UIManager>(input);

    runtime.addResource<Canvas>(renderer);
    runtime.addResource(m_manager.get());

    runtime.addSystem<core::FrameBegin>("UIManager::Update", UIUpdateSystem);
    runtime.addSystem<core::Render>("UIManager::Render", UIRenderSystem);
}
