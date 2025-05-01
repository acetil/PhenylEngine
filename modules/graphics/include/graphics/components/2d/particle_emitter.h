#pragma once

#include "core/assets/asset.h"
#include "core/serialization/serializer_forward.h"
#include "core/component/forward.h"
#include "graphics/maths_headers.h"
#include "core/delta_time.h"
#include "core/runtime.h"

namespace phenyl::core {
    class GlobalTransform2D;
}

namespace phenyl::graphics {
    class ParticleSystem2D;

    struct ParticleEmitter2D {
    public:
        phenyl::core::Asset<ParticleSystem2D> system;
        glm::vec2 direction{1.0f, 0.0f};

        float duration;
        float explosiveness;

        std::size_t particlesPerLoop;
        std::size_t particlesEmitted{0};
        float timeInLoop{0.0f};

        bool oneShot = false;
        bool enabled = true;

        static void AddSystems (core::PhenylRuntime& runtime, core::System<core::Update>& particleUpdateSystem);

        void update (const core::Resources<const core::DeltaTime>& resources, const core::GlobalTransform2D& transform);
        void start ();

    private:
        void updateInternal (double deltaTime, const core::GlobalTransform2D& transform);
    };

    PHENYL_DECLARE_SERIALIZABLE(ParticleEmitter2D)
}