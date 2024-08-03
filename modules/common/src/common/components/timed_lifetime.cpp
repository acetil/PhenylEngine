#include "component/component.h"

#include "common/components/timed_lifetime.h"
#include "common/components/timed_lifetime_serialize.h"
#include "runtime/delta_time.h"

using namespace phenyl;

static void UpdateSystem (component::Entity entity, const runtime::Resources<runtime::DeltaTime>& resources, common::TimedLifetime& comp) {
    comp.livedTime += resources.get<runtime::DeltaTime>()();
    if (comp.livedTime >= comp.lifetime) {
        entity.remove();
    }
}

void common::TimedLifetime::Init (runtime::PhenylRuntime& runtime) {
    runtime.addComponent<TimedLifetime>();
    runtime.addSystem<runtime::Update>("TimedLifetime::Update", UpdateSystem);
}

