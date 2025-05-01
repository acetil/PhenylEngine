#pragma once

#include "core/iresource.h"

namespace phenyl::graphics {
    class ParticleManager2D;
    class ParticleRenderLayer;

    class Particle2DPlugin : public core::IPlugin {
    private:
        std::unique_ptr<ParticleManager2D> m_manager;
        ParticleRenderLayer* m_layer = nullptr;
    public:
        Particle2DPlugin();
        ~Particle2DPlugin() override;

        [[nodiscard]] std::string_view getName () const noexcept override;
        void init (core::PhenylRuntime& runtime) override;
    };
}