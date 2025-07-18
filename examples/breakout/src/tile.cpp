#include "tile.h"

#include "controller.h"

#include <phenyl/components/2D/global_transform.h>
#include <phenyl/components/2D/particle_emitter.h>
#include <phenyl/components/audio_player.h>
#include <phenyl/signals/physics.h>

using namespace breakout;

namespace breakout {
PHENYL_SERIALIZABLE(Tile, PHENYL_SERIALIZABLE_MEMBER(points), PHENYL_SERIALIZABLE_MEMBER(health),
    PHENYL_SERIALIZABLE_MEMBER(emitter), PHENYL_SERIALIZABLE_MEMBER_NAMED(breakSample, "break_sample"))

PHENYL_SERIALIZABLE(Floor, PHENYL_SERIALIZABLE_MEMBER(sample))
} // namespace breakout

void breakout::InitTile (breakout::BreakoutApp* app, phenyl::World& world) {
    app->addComponent<Tile>("Tile");
    app->addComponent<Floor>("Floor");

    world.addHandler<phenyl::signals::OnCollision, const phenyl::GlobalTransform2D, Tile>(
        [app] (const phenyl::signals::OnCollision& signal,
            const phenyl::Bundle<const phenyl::GlobalTransform2D, Tile>& bundle) {
            auto& [transform, tile] = bundle.comps();
            auto entity = bundle.entity();
            if (!--tile.health) {
                // app->addPoints(tile.points);
                entity.parent().raise(OnTileBreak{tile.points});

                /*tile.emitter->instantiate()
                    .with(emitterTransform)
                    .complete()
                    .apply<phenyl::ParticleEmitter2D>([normal=signal.normal]
                   (phenyl::ParticleEmitter2D& emitter) { emitter.direction = -normal;
                    })
                    .apply<phenyl::AudioPlayer>([sample=tile.breakSample] (phenyl::AudioPlayer&
                   player) { player.play(sample);
                    });*/
                auto emitterEntity = entity.world().create();
                emitterEntity.insert(phenyl::Transform2D{}.setPosition(transform.position()));
                tile.emitter->instantiate(emitterEntity);
                emitterEntity.apply<phenyl::ParticleEmitter2D>(
                    [normal = signal.normal] (phenyl::ParticleEmitter2D& emitter) { emitter.direction = -normal; });
                emitterEntity.apply<phenyl::AudioPlayer>(
                    [sample = tile.breakSample] (phenyl::AudioPlayer& player) { player.play(*sample); });

                entity.remove();
            }
        });

    world.addHandler<phenyl::signals::OnCollision, const Floor, phenyl::AudioPlayer>(
        [] (const phenyl::signals::OnCollision& signal, const Floor& floor, phenyl::AudioPlayer& player) {
            player.play(*floor.sample);
        });
}
