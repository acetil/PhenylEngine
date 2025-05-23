#pragma once

#include "core/assets/asset_manager.h"
#include "core/iresource.h"
#include "graphics/particles/particle_system_2d.h"

namespace phenyl::graphics {
class ParticleManager2D : public phenyl::core::AssetManager<ParticleSystem2D>, public core::IResource {
public:
    explicit ParticleManager2D (std::size_t systemMaxParticles);

    void selfRegister ();

    void update (float deltaTime);

    void buffer (Buffer<glm::vec2>& posBuffer, Buffer<glm::vec4>& colourBuffer) const;

    [[nodiscard]] std::string_view getName () const noexcept override;

protected:
    [[nodiscard]] const char* getFileType () const override;

    ParticleSystem2D* load (std::ifstream& data, std::size_t id) override;
    ParticleSystem2D* load (phenyl::graphics::ParticleSystem2D&& obj, std::size_t id) override;
    void queueUnload (std::size_t id) override;

private:
    std::unordered_map<std::size_t, std::unique_ptr<ParticleSystem2D>> m_systems;
    std::size_t m_maxParticles;
};
} // namespace phenyl::graphics
