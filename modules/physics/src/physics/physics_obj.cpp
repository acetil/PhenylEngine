#include "physics/physics.h"
#include "2d/physics_obj_2d.h"

using namespace physics;


std::unique_ptr<IPhysics> physics::makeDefaultPhysics () {
    return std::make_unique<PhysicsObject2D>();
}
