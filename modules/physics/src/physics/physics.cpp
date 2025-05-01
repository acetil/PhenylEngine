#include "physics/physics.h"
#include "2d/physics_2d.h"
#include "core/debug.h"
#include "core/plugins/core_plugin_2d.h"
#include "core/runtime.h"

using namespace phenyl;

physics::Physics2DPlugin::Physics2DPlugin () : m_physics{std::make_unique<Physics2D>()} {}
physics::Physics2DPlugin::~Physics2DPlugin () = default;


std::string_view physics::Physics2DPlugin::getName () const noexcept {
    return "PhysicsPlugin";
}

void physics::Physics2DPlugin::init (core::PhenylRuntime& runtime) {
    runtime.addPlugin<core::Core2DPlugin>();

    m_physics->addComponents(runtime);
    runtime.addSystem<core::Render>("Physics2d::DebugRender", this, &Physics2DPlugin::render);
}

void physics::Physics2DPlugin::render (core::PhenylRuntime& runtime) {
    auto* config = runtime.resourceMaybe<core::DebugRenderConfig>();
    if (config && config->doPhysicsRender) {
        m_physics->debugRender(runtime.world());
    }
}
