#pragma once

#include "util/map.h"

#include "graphics/particles/particle_system_2d.h"
#include "common/assets/asset_manager.h"

namespace phenyl::graphics {
    class ParticleManager2D : public phenyl::common::AssetManager<ParticleSystem2D> {
    private:
        util::Map<std::size_t, std::unique_ptr<ParticleSystem2D>> systems;
        std::size_t systemMaxParticles;
    protected:
        [[nodiscard]] const char* getFileType () const override;

        ParticleSystem2D* load (std::istream& data, std::size_t id) override;
        ParticleSystem2D* load (phenyl::graphics::ParticleSystem2D&& obj, std::size_t id) override;
        void queueUnload (std::size_t id) override;
    public:
        explicit ParticleManager2D (std::size_t systemMaxParticles);

        void selfRegister ();

        void update (float deltaTime);

        void buffer (Buffer<glm::vec2>& posBuffer, Buffer<glm::vec4>& colourBuffer) const;
    };
}