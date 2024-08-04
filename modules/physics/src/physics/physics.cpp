#include "physics/physics.h"
#include "2d/physics_2d.h"
#include "common/debug.h"
#include "common/plugins/core_plugin_2d.h"

using namespace phenyl;

physics::Physics2DPlugin::Physics2DPlugin () : physics{std::make_unique<Physics2D>()} {}
physics::Physics2DPlugin::~Physics2DPlugin () = default;


std::string_view physics::Physics2DPlugin::getName () const noexcept {
    return "PhysicsPlugin";
}

void physics::Physics2DPlugin::init (runtime::PhenylRuntime& runtime) {
    runtime.addPlugin<common::Core2DPlugin>();

    physics->addComponents(runtime);
    runtime.addSystem<runtime::Render>("Physics2d::DebugRender", this, &Physics2DPlugin::render);
}

void physics::Physics2DPlugin::render (runtime::PhenylRuntime& runtime) {
    auto* config = runtime.resourceMaybe<common::DebugRenderConfig>();
    if (config && config->doPhysicsRender) {
        physics->debugRender(runtime.manager());
    }
}
