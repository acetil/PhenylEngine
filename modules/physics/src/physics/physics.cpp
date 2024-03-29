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
}

void physics::Physics2DPlugin::physicsUpdate (runtime::PhenylRuntime& runtime, double deltaTime) {
    physics->updatePhysics(runtime.manager(), (float)deltaTime);
}

void physics::Physics2DPlugin::render (runtime::PhenylRuntime& runtime) {
    auto* config = runtime.resourceMaybe<common::DebugRenderConfig>();
    if (config && config->doPhysicsRender) {
        physics->debugRender(runtime.manager());
    }
}
