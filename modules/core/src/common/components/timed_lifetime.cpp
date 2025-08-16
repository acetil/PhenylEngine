#include "core/components/timed_lifetime.h"

#include "core/clock.h"
#include "core/serialization/serializer_impl.h"

using namespace phenyl;

namespace phenyl::core {
PHENYL_SERIALIZABLE(TimedLifetime, PHENYL_SERIALIZABLE_MEMBER(lifetime))
}

static void UpdateSystem (const core::Resources<core::Clock>& resources,
    const core::Bundle<core::TimedLifetime>& bundle) {
    auto& [comp] = bundle.comps();
    comp.livedTime += resources.get<core::Clock>().deltaTime();
    if (comp.livedTime >= comp.lifetime) {
        bundle.entity().remove();
    }
}

void core::TimedLifetime::Init (PhenylRuntime& runtime) {
    runtime.addComponent<TimedLifetime>("TimedLifetime");
    runtime.addSystem<core::Update>("TimedLifetime::Update", UpdateSystem);
}
