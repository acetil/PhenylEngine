#pragma once

#include "common/assets/asset.h"
#include "component/forward.h"
#include "graphics/maths_headers.h"
#include "runtime/delta_time.h"
#include "runtime/runtime.h"

namespace phenyl::common {
    class GlobalTransform2D;
}

namespace phenyl::graphics {
    class ParticleSystem2D;

    struct ParticleEmitter2D {
    private:
        void updateInternal (double deltaTime, const common::GlobalTransform2D& transform);
    public:
        phenyl::common::Asset<ParticleSystem2D> system;
        glm::vec2 direction{1.0f, 0.0f};

        float duration;
        float explosiveness;

        std::size_t particlesPerLoop;
        std::size_t particlesEmitted{0};
        float timeInLoop{0.0f};

        bool oneShot = false;
        bool enabled = true;

        static void AddSystems (runtime::PhenylRuntime& runtime, runtime::System<runtime::Update>& particleUpdateSystem);

        void update (const runtime::Resources<const runtime::DeltaTime>& resources, const common::GlobalTransform2D& transform);
        void start ();
    };
}