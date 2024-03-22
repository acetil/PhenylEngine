#include "graphics/components/2d/particle_emitter.h"
#include "graphics/components/2d/particle_emitter_serialization.h"
#include "graphics/particles/particle_plugin.h"
#include "graphics/particles/particle_manager.h"
#include "graphics/renderers/renderer.h"
#include "graphics/graphics.h"
#include "graphics/renderlayer/particle_layer.h"

using namespace phenyl::graphics;

Particle2DPlugin::Particle2DPlugin () = default;
Particle2DPlugin::~Particle2DPlugin () = default;

std::string_view Particle2DPlugin::getName () const noexcept {
    return "Particle2DPlugin";
}

void Particle2DPlugin::init (runtime::PhenylRuntime& runtime) {
    auto& graphics = runtime.resource<detail::Graphics>();
    auto& renderer = runtime.resource<Renderer>();

    runtime.addComponent<ParticleEmitter2D>();

    manager = std::make_unique<ParticleManager2D>(256);
    manager->selfRegister();

    graphics.getRenderLayer()->addRenderLayer(std::make_shared<ParticleRenderLayer>(&renderer, manager.get()));
}

void Particle2DPlugin::update (runtime::PhenylRuntime& runtime, double deltaTime) {
    manager->update((float)deltaTime);
    ParticleEmitter2D::Update((float)deltaTime, runtime.manager());
}


