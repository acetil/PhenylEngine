#include "core/plugins/core_plugin.h"

#include "core/debug.h"
#include "core/runtime.h"

using namespace phenyl::core;

std::string_view CorePlugin::getName () const noexcept {
    return "CorePlugin";
}

void CorePlugin::init (PhenylRuntime& runtime) {
    runtime.addResource<Debug>();
}
