#include "phenyl/plugins/default_2d_plugin.h"

#include "audio/audio_plugin.h"
#include "core/plugins/core_plugin_2d.h"
#include "core/plugins/input_plugin.h"
#include "core/plugins/timed_lifetime_plugin.h"
#include "core/runtime.h"
#include "graphics/plugins/graphics_2d_plugin.h"
#include "phenyl/plugins/level_plugin.h"
#include "phenyl/plugins/prefab_plugin.h"
#include "physics/physics.h"
#include "ui/debug_ui.h"
#include "ui/plugins/ui_plugin.h"

using namespace phenyl;

std::string_view Default2DPlugin::getName () const noexcept {
    return "Default2DPlugin";
}

void Default2DPlugin::init (PhenylRuntime& runtime) {
    runtime.addPlugin<LevelPlugin>();

    runtime.addPlugin<core::Core2DPlugin>();
    runtime.addPlugin<graphics::Graphics2DPlugin>();
    runtime.addPlugin<physics::Physics2DPlugin>();
    runtime.addPlugin<audio::AudioPlugin>();
    runtime.addPlugin<graphics::UIPlugin>();
    runtime.addPlugin<graphics::ProfileUiPlugin>();
    runtime.addPlugin<core::InputPlugin>();

    // TODO: move when adding more default components
    runtime.addPlugin<core::TimedLifetimePlugin>();
}
