#include "graphics/plugins/particle_plugin.h"

#include "graphics/backend/renderer.h"
#include "graphics/components/2d/particle_emitter.h"
#include "graphics/particles/particle_manager.h"
#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/particle_layer.h"

using namespace phenyl::graphics;

struct ParticleData : public phenyl::core::IResource {
    ParticleRenderLayer* layer;

    ParticleData (ParticleRenderLayer* layer) : layer{layer} {}

    [[nodiscard]] std::string_view getName () const noexcept override {
        return "ParticleData";
    }
};

static void UpdateSystem (const phenyl::core::Resources<const phenyl::core::DeltaTime, ParticleManager2D>& resources) {
    auto& [deltaTime, manager] = resources;
    manager.update(static_cast<float>(deltaTime()));
}

static void RenderSystem (const phenyl::core::Resources<ParticleData, Camera2D, ParticleManager2D>& resources) {
    auto& [data, camera, manager] = resources;
    data.layer->bufferData(camera, manager);
}

Particle2DPlugin::Particle2DPlugin () = default;
Particle2DPlugin::~Particle2DPlugin () = default;

std::string_view Particle2DPlugin::getName () const noexcept {
    return "Particle2DPlugin";
}

void Particle2DPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();

    auto& renderer = runtime.resource<Renderer>();

    runtime.addComponent<ParticleEmitter2D>("ParticleEmitter2D");

    // manager = std::make_unique<ParticleManager2D>(256);
    // manager->selfRegister();
    runtime.addResource<ParticleManager2D>(256);
    runtime.resource<ParticleManager2D>().selfRegister();

    m_layer = &renderer.addLayer<ParticleRenderLayer>();
    runtime.addResource<ParticleData>(m_layer);

    auto& particleUpdateSystem = runtime.addSystem<phenyl::core::Update>("ParticleManager::Update", UpdateSystem);
    ParticleEmitter2D::AddSystems(runtime, particleUpdateSystem);

    runtime.addSystem<phenyl::core::Render>("ParticleManager::Render", RenderSystem);
}
