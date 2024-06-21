#include "graphics/renderer.h"
#include "graphics/components/2d/particle_emitter.h"
#include "graphics/components/2d/particle_emitter_serialization.h"
#include "graphics/particles/particle_manager.h"
#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/particle_layer.h"

#include "graphics/plugins/particle_plugin.h"

using namespace phenyl::graphics;

struct ParticleData : public phenyl::runtime::IResource {
    ParticleRenderLayer* layer;

    ParticleData (ParticleRenderLayer* layer) : layer{layer} {}

    [[nodiscard]] std::string_view getName() const noexcept override {
        return "ParticleData";
    }
};

static void UpdateSystem (const phenyl::runtime::Resources<const phenyl::runtime::DeltaTime, ParticleManager2D>& resources) {
    auto& [deltaTime, manager] = resources;
    manager.update(static_cast<float>(deltaTime()));
}

static void RenderSystem (const phenyl::runtime::Resources<ParticleData, ParticleManager2D>& resources) {
    auto& [data, manager] = resources;
    data.layer->bufferData(manager);
}

Particle2DPlugin::Particle2DPlugin () = default;
Particle2DPlugin::~Particle2DPlugin () = default;

std::string_view Particle2DPlugin::getName () const noexcept {
    return "Particle2DPlugin";
}

void Particle2DPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();

    auto& renderer = runtime.resource<Renderer>();

    runtime.addComponent<ParticleEmitter2D>();

    //manager = std::make_unique<ParticleManager2D>(256);
    //manager->selfRegister();
    runtime.addResource<ParticleManager2D>(256);
    runtime.resource<ParticleManager2D>().selfRegister();

    layer = &renderer.addLayer<ParticleRenderLayer>();
    runtime.addResource<ParticleData>(layer);

    runtime.addSystem<phenyl::runtime::Update>(UpdateSystem);
    ParticleEmitter2D::AddSystems(runtime);

    runtime.addSystem<phenyl::runtime::Render>(RenderSystem);
}

void Particle2DPlugin::update (runtime::PhenylRuntime& runtime, double deltaTime) {
    //runtime.resource<ParticleManager2D>().update((float)deltaTime);
    //ParticleEmitter2D::Update((float)deltaTime, runtime.manager());
}

void Particle2DPlugin::render (runtime::PhenylRuntime& runtime) {
    //PHENYL_DASSERT(layer);
    //layer->bufferData(*manager);
}


