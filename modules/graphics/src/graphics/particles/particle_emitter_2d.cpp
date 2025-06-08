#include "core/assets/assets.h"
#include "core/components/2d/global_transform.h"
#include "core/runtime.h"
#include "core/serialization/serializer_impl.h"
#include "graphics/components/2d/particle_emitter.h"
#include "graphics/particles/particle_system_2d.h"

using namespace phenyl::graphics;

namespace phenyl::graphics {
PHENYL_SERIALIZABLE(ParticleEmitter2D, PHENYL_SERIALIZABLE_MEMBER(system), PHENYL_SERIALIZABLE_MEMBER(direction),
    PHENYL_SERIALIZABLE_MEMBER(duration), PHENYL_SERIALIZABLE_MEMBER(explosiveness),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(particlesPerLoop, "num_particles"),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(oneShot, "one_shot"), PHENYL_SERIALIZABLE_MEMBER(enabled))
}

void ParticleEmitter2D::update (const core::Resources<const core::DeltaTime>& resources,
    const core::GlobalTransform2D& transform) {
    updateInternal(resources.get<const core::DeltaTime>()(), transform);
}

void ParticleEmitter2D::updateInternal (double deltaTime, const core::GlobalTransform2D& transform) {
    if (!enabled) {
        return;
    }

    auto timeRemaining = duration - timeInLoop;
    timeInLoop += std::min(timeRemaining, static_cast<float>(deltaTime));

    while (particlesEmitted < particlesPerLoop &&
        (float) particlesEmitted * (duration / (float) particlesPerLoop) * (1 - explosiveness) <= timeInLoop) {
        system->emit(transform.position(), transform.transform.rotation() * direction);
        particlesEmitted++;
    }

    if (std::abs(duration - timeInLoop) < std::numeric_limits<float>::epsilon()) {
        if (oneShot) {
            timeInLoop = 0.0f;
            enabled = false;
            return;
        }

        timeInLoop = 0.0f;
        particlesEmitted = 0;
        updateInternal(deltaTime - timeRemaining, transform);
    }
}

void ParticleEmitter2D::AddSystems (core::PhenylRuntime& runtime, core::System<core::Update>& particleUpdateSystem) {
    runtime.addSystem<core::Update>("ParticleEmitter2D::Update", &ParticleEmitter2D::update)
        .runBefore(particleUpdateSystem);
}

void ParticleEmitter2D::start () {
    particlesEmitted = 0;
    enabled = true;
    timeInLoop = 0;
}
