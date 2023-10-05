#include "common/components/2d/global_transform.h"
#include "component/component.h"

#include "graphics/components/2d/particle_emitter.h"
#include "graphics/particles/particle_system_2d.h"
#include "graphics/components/2d/particle_emitter_serialization.h"

using namespace phenyl::graphics;

inline static void update (const phenyl::common::GlobalTransform2D& transform, ParticleEmitter2D& emitter, float deltaTime) {
    if (!emitter.enabled) {
        return;
    }

    auto timeRemaining = emitter.duration - emitter.timeInLoop;
    emitter.timeInLoop += std::min(timeRemaining, deltaTime);

    while (emitter.particlesEmitted < emitter.particlesPerLoop && (float)emitter.particlesEmitted * (emitter.duration / (float)emitter.particlesPerLoop) * (1 - emitter.explosiveness) <= emitter.timeInLoop) {
        emitter.system->emit(transform.transform2D.position(), transform.transform2D.rotMatrix() * emitter.direction);
        emitter.particlesEmitted++;
    }

    if (std::abs(emitter.duration - emitter.timeInLoop) < std::numeric_limits<float>::epsilon()) {
        if (emitter.oneShot) {
            emitter.timeInLoop = 0.0f;
            emitter.enabled = false;
            return;
        }

        emitter.timeInLoop = 0.0f;
        emitter.particlesEmitted = 0;
        update(transform, emitter, deltaTime - timeRemaining);
    }
}

void ParticleEmitter2D::Update (float deltaTime, phenyl::component::ComponentManager& componentManager) {
    componentManager.query<const phenyl::common::GlobalTransform2D, ParticleEmitter2D>().each([deltaTime] (auto entity, const phenyl::common::GlobalTransform2D& transform, ParticleEmitter2D& emitter) {
        update(transform, emitter, deltaTime);
    });
}

void ParticleEmitter2D::start () {
    particlesEmitted = 0;
    enabled = true;
    timeInLoop = 0;
}
