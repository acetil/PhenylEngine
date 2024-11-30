#include "graphics/renderer.h"

#include "core/input/game_input.h"
#include "core/plugins/input_plugin.h"
#include "core/runtime.h"


using namespace phenyl::core;

static void InputUpdateSystem (const Resources<GameInput>& resources) {
    resources.get<GameInput>().update();
}

std::string_view InputPlugin::getName () const noexcept {
    return "InputPlugin";
}

void InputPlugin::init (PhenylRuntime& runtime) {
    runtime.addResource<GameInput>();

    auto& input = runtime.resource<GameInput>();
    auto& renderer = runtime.resource<graphics::Renderer>();
    renderer.getViewport().addInputDevices(input);

    runtime.addSystem<FrameBegin>("GameInput::Update", InputUpdateSystem);
}
