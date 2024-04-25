#include "phenyl/plugins/default_2d_plugin.h"

#include "runtime/runtime.h"
#include "phenyl/plugins/level_plugin.h"
#include "phenyl/plugins/prefab_plugin.h"
#include "graphics/plugins/graphics_2d_plugin.h"
#include "physics/physics.h"
#include "audio/audio_plugin.h"
#include "graphics/ui/debug_ui.h"
#include "common/plugins/core_plugin_2d.h"
#include "common/plugins/input_plugin.h"
#include "common/plugins/timed_lifetime_plugin.h"

using namespace phenyl;

std::string_view Default2DPlugin::getName () const noexcept {
    return "Default2DPlugin";
}

void Default2DPlugin::init (PhenylRuntime& runtime) {
    runtime.addPlugin<LevelPlugin>();

    runtime.addPlugin<common::Core2DPlugin>();
    runtime.addPlugin<graphics::Graphics2DPlugin>();
    runtime.addPlugin<physics::Physics2DPlugin>();
    runtime.addPlugin<audio::AudioPlugin>();
    runtime.addPlugin<graphics::ProfileUiPlugin>();
    runtime.addPlugin<common::InputPlugin>();

    // TODO: move when adding more default components
    runtime.addPlugin<common::TimedLifetimePlugin>();
}