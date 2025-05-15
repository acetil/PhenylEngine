#include "core/plugins/timed_lifetime_plugin.h"

#include "core/components/timed_lifetime.h"

using namespace phenyl::core;

std::string_view TimedLifetimePlugin::getName () const noexcept {
    return "TimedLifetimePlugin";
}

void TimedLifetimePlugin::init (PhenylRuntime& runtime) {
    TimedLifetime::Init(runtime);
}
