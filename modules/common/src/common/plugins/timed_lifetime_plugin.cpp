#include "common/plugins/timed_lifetime_plugin.h"
#include "common/components/timed_lifetime.h"

using namespace phenyl::common;

std::string_view TimedLifetimePlugin::getName () const noexcept {
    return "TimedLifetimePlugin";
}

void TimedLifetimePlugin::init (runtime::PhenylRuntime& runtime) {
    TimedLifetime::Init(runtime);
}
