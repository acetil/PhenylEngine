#include "component/component.h"
#include "physics/signals/collision.h"
#include "serializers.h"
#include "component/component_serializer.h"

#include "bullet.h"

using namespace phenyl;

void test::addBulletSignals (component::ComponentManager& manager, component::EntitySerializer& serialiser) {
    manager.addComponent<Bullet>();

    serialiser.addSerializer<Bullet>();

    manager.handleSignal<physics::OnCollision, Bullet>([] (const physics::OnCollision& signal, component::Entity entity, Bullet& bullet) {
        //info.manager().remove(info.id()); // TODO: queue
        //info.manager().entity(info.id()).remove();
        entity.remove();
    });
}