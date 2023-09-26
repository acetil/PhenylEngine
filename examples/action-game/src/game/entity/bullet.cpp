#include <phenyl/component.h>
#include <phenyl/signals/physics.h>

#include "bullet.h"
#include "serializers.h"


using namespace phenyl;

void test::addBulletSignals (phenyl::ComponentManager& manager, phenyl::ComponentSerializer& serialiser) {
    manager.addComponent<Bullet>();

    serialiser.addSerializer<Bullet>();

    manager.handleSignal<physics::OnCollision, Bullet>([] (const physics::OnCollision& signal, phenyl::Entity entity, Bullet& bullet) {
        //info.manager().remove(info.id()); // TODO: queue
        //info.manager().entity(info.id()).remove();
        entity.remove();
    });
}