#include "graphics/renderer.h"
#include "graphics/components/2d/particle_emitter.h"
#include "graphics/components/2d/particle_emitter_serialization.h"
#include "graphics/particles/particle_manager.h"
#include "graphics/plugins/graphics_plugin.h"
#include "graphics/renderlayer/particle_layer.h"

#include "graphics/plugins/particle_plugin.h"

using namespace phenyl::graphics;

Particle2DPlugin::Particle2DPlugin () = default;
Particle2DPlugin::~Particle2DPlugin () = default;

std::string_view Particle2DPlugin::getName () const noexcept {
    return "Particle2DPlugin";
}

void Particle2DPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<GraphicsPlugin>();

    auto& renderer = runtime.resource<Renderer>();

    runtime.addComponent<ParticleEmitter2D>();

    manager = std::make_unique<ParticleManager2D>(256);
    manager->selfRegister();

    layer = &renderer.addLayer<ParticleRenderLayer>();
}

void Particle2DPlugin::update (runtime::PhenylRuntime& runtime, double deltaTime) {
    manager->update((float)deltaTime);
    ParticleEmitter2D::Update((float)deltaTime, runtime.manager());
}

void Particle2DPlugin::render (runtime::PhenylRuntime& runtime) {
    PHENYL_DASSERT(layer);
    layer->bufferData(*manager);
}


