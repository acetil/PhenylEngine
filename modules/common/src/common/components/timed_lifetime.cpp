#include "component/component.h"

#include "common/components/timed_lifetime.h"
#include "common/components/timed_lifetime_serialize.h"

void phenyl::common::TimedLifetime::Update (phenyl::component::ComponentManager& componentManager, double deltaTime) {
    componentManager.query<TimedLifetime>().each([deltaTime] (component::Entity entity, TimedLifetime& comp) {
        comp.livedTime += deltaTime;
        if (comp.livedTime >= comp.lifetime) {
            entity.remove();
        }
    });
}
