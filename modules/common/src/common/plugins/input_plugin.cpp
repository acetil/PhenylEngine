#include "graphics/renderer.h"

#include "common/input/game_input.h"
#include "common/plugins/input_plugin.h"


using namespace phenyl::common;

std::string_view InputPlugin::getName () const noexcept {
    return "InputPlugin";
}

void InputPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addResource<GameInput>();

    auto& input = runtime.resource<GameInput>();
    auto& renderer = runtime.resource<graphics::Renderer>();
    renderer.getViewport().addInputDevices(input);
}

void InputPlugin::frameBegin (runtime::PhenylRuntime& runtime) {
    runtime.resource<GameInput>().update();
}
