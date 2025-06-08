#include "bullet.h"

#include "game/test_app.h"

#include <phenyl/components/2D/global_transform.h>
#include <phenyl/signals/physics.h>
#include <phenyl/world.h>

using namespace phenyl;

namespace test {
PHENYL_SERIALIZABLE(Bullet, PHENYL_SERIALIZABLE_MEMBER_NAMED(particlePrefab, "particle_prefab"))
}

void test::InitBullet (TestApp* app, World& world) {
    app->addComponent<Bullet>("Bullet");

    world.addHandler<physics::OnCollision, Bullet, const phenyl::GlobalTransform2D, const Transform2D>(
        [] (const physics::OnCollision& signal,
            const phenyl::Bundle<Bullet, const phenyl::GlobalTransform2D, const phenyl::Transform2D>& bundle) {
            auto& [bullet, globalTransform, transform] = bundle.comps();

            Transform2D particleTransform{};
            particleTransform.setPosition(globalTransform.position()).setRotation(transform.rotationAngle());

            auto particleEntity = bundle.entity().world().create();
            particleEntity.insert(particleTransform);
            bullet.particlePrefab->instantiate(particleEntity);

            bundle.entity().remove();
        });
}
