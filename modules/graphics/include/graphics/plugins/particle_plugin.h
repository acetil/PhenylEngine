#pragma once

#include "runtime/plugin.h"

namespace phenyl::graphics {
    class ParticleManager2D;
    class ParticleRenderLayer;

    class Particle2DPlugin : public runtime::IPlugin {
    private:
        std::unique_ptr<ParticleManager2D> manager;
        ParticleRenderLayer* layer = nullptr;
    public:
        Particle2DPlugin();
        ~Particle2DPlugin() override;

        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (runtime::PhenylRuntime& runtime) override;
    };
}