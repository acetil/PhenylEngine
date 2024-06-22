#include "common/components/timed_lifetime_serialize.h"
#include "common/plugins/timed_lifetime_plugin.h"

using namespace phenyl::common;

std::string_view TimedLifetimePlugin::getName () const noexcept {
    return "TimedLifetimePlugin";
}

void TimedLifetimePlugin::init (runtime::PhenylRuntime& runtime) {
    TimedLifetime::Init(runtime);
}
