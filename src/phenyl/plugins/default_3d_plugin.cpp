#include "phenyl/plugins/default_3d_plugin.h"

#include "audio/audio_plugin.h"
#include "core/plugins/core_plugin_3d.h"
#include "core/plugins/input_plugin.h"
#include "core/plugins/timed_lifetime_plugin.h"
#include "core/runtime.h"
#include "graphics/plugins/graphics_3d_plugin.h"
#include "graphics/ui/debug_ui.h"
#include "phenyl/plugins/level_plugin.h"
#include "physics/physics.h"

using namespace phenyl;

std::string_view Default3DPlugin::getName () const noexcept {
    return "Default3DPlugin";
}

void Default3DPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addPlugin<LevelPlugin>();

    runtime.addPlugin<core::Core3DPlugin>();
    runtime.addPlugin<graphics::Graphics3DPlugin>();
    // runtime.addPlugin<physics::Physics2DPlugin>();
    runtime.addPlugin<audio::AudioPlugin>();
    runtime.addPlugin<graphics::ProfileUiPlugin>();
    runtime.addPlugin<core::InputPlugin>();

    // TODO: move when adding more default components
    runtime.addPlugin<core::TimedLifetimePlugin>();
}
