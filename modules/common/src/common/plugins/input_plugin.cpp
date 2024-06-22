#include "graphics/renderer.h"

#include "common/input/game_input.h"
#include "common/plugins/input_plugin.h"


using namespace phenyl::common;

static void InputUpdateSystem (const phenyl::runtime::Resources<GameInput>& resources) {
    resources.get<GameInput>().update();
}

std::string_view InputPlugin::getName () const noexcept {
    return "InputPlugin";
}

void InputPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addResource<GameInput>();

    auto& input = runtime.resource<GameInput>();
    auto& renderer = runtime.resource<graphics::Renderer>();
    renderer.getViewport().addInputDevices(input);

    runtime.addSystem<phenyl::runtime::FrameBegin>(InputUpdateSystem);
}
