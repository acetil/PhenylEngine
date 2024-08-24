#include "common/serializer_impl.h"

#include "common/components/timed_lifetime.h"
#include "runtime/delta_time.h"

using namespace phenyl;

namespace phenyl::common {
    PHENYL_SERIALIZABLE(TimedLifetime, PHENYL_SERIALIZABLE_MEMBER(lifetime))
}

static void UpdateSystem (const runtime::Resources<runtime::DeltaTime>& resources, const component::Bundle<common::TimedLifetime>& bundle) {
    auto& [comp] = bundle.comps();
    comp.livedTime += resources.get<runtime::DeltaTime>()();
    if (comp.livedTime >= comp.lifetime) {
        bundle.entity().remove();
    }
}

void common::TimedLifetime::Init (runtime::PhenylRuntime& runtime) {
    runtime.addComponent<TimedLifetime>("TimedLifetime");
    runtime.addSystem<runtime::Update>("TimedLifetime::Update", UpdateSystem);
}

