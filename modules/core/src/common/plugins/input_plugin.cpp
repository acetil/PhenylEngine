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
    runtime.addSystem<FrameBegin>("GameInput::Update", InputUpdateSystem);
}
