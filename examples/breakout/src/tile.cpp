#include <iostream>

#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/particle_emitter.h>
#include <phenyl/signals/physics.h>

#include "tile.h"

using namespace breakout;

void breakout::initTile (breakout::BreakoutApp* app, phenyl::ComponentManager& componentManager) {
    app->addComponent<Tile>();

    componentManager.handleSignal<phenyl::signals::OnCollision, const phenyl::GlobalTransform2D, Tile>([app] (const phenyl::signals::OnCollision& signal, phenyl::Entity entity, const phenyl::GlobalTransform2D& transform, Tile& tile) {
        if (!--tile.health) {
            app->addPoints(tile.points);

            phenyl::GlobalTransform2D emitterTransform{};
            emitterTransform.transform2D.setPosition(transform.transform2D.position());
            tile.emitter->instantiate()
                .with(emitterTransform)
                .complete()
                .apply<phenyl::ParticleEmitter2D>([normal=signal.normal] (phenyl::ParticleEmitter2D& emitter) {
                    emitter.direction = -normal;
                });

            entity.remove();
        }
    });
}