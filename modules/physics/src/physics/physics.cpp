#include "physics/physics.h"
#include "2d/physics_2d.h"
#include "common/debug.h"

using namespace phenyl;

physics::PhysicsPlugin2D::PhysicsPlugin2D () : physics{std::make_unique<Physics2D>()} {}
physics::PhysicsPlugin2D::~PhysicsPlugin2D () = default;


std::string_view physics::PhysicsPlugin2D::getName () const noexcept {
    return "PhysicsPlugin";
}

void physics::PhysicsPlugin2D::init (runtime::PhenylRuntime& runtime) {
    physics->addComponents(runtime);
}

void physics::PhysicsPlugin2D::physicsUpdate (runtime::PhenylRuntime& runtime, double deltaTime) {
    physics->updatePhysics(runtime.manager(), (float)deltaTime);
}

void physics::PhysicsPlugin2D::render (runtime::PhenylRuntime& runtime) {
    if (runtime.resource<common::DebugRenderConfig>().doPhysicsRender) {
        physics->debugRender(runtime.manager());
    }
}
