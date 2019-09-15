#include <vector>
#include <algorithm>

#include "physics.h"
#include "physics_body.h"
#include "physics_equations.h"
#include "logging/logging.h"
using namespace physics;

#define X_FRICTION_TAG 0
#define Y_FRICTION_TAG 1
void physics::PhysicsBody::updatePosition (float time) {
    /*if (velX == 0 || velY == 0) {
        forceUpdate = true;
    }*/
    if (forceUpdate) {
        updateForceEquations();
        forceUpdate = false;
    }
    //logging::log(LEVEL_DEBUG, "Updating positions!");
    // TODO: account for intercepts with v = 0
    *x = xEquation->getPosition(time);
    *y = yEquation->getPosition(time);
    velX = xEquation->getVelocity(time);
    velY = yEquation->getVelocity(time);

    xEquation->updateTime(time);
    yEquation->updateTime(time);
}

void physics::PhysicsBody::addForce (Force f) {
    forces.push_back(f);
    forceUpdate = true;
}
void physics::PhysicsBody::removeMovementForces (int tag) {
    // tag must be >= 0, if < 0 all will be removed
    forces.erase(std::remove_if(forces.begin(), forces.end(), [tag](const Force& f){
        return f.type == MOVEMENT_FORCE_TYPE && (tag == f.tag || tag < 0);
    }), forces.end());
    forceUpdate = true;
}
void physics::PhysicsBody::setVelocity (float velX, float velY) {
    this->velX = velX;
    this->velY = velY;
    forceUpdate = true;
}
physics::PhysicsBody::PhysicsBody (float* x, float* y, float mass, float constFrictionK, float linearFrictionK) {
    this->x = x;
    this->y = y;
    this->velX = 0;
    this->velY = 0;
    this->mass = mass;
    this->constFrictionK = constFrictionK;
    this->linearFrictionK = linearFrictionK;
    forces.push_back(Force(-1 * linearFrictionK, -1 * linearFrictionK, LINEAR_DEGREE, FRICTION_FORCE_TYPE, 2, -1));
}
float physics::PhysicsBody::getNetForce (int direction) {
    float netForce = 0;
    for (auto const& x : forces) {
        if (x.degree == CONSTANT_DEGREE) {
            if (direction == X_COMP) {
                netForce += x.xComponent;
            } else if (direction == Y_COMP) {
                netForce += x.yComponent;
            }
        }
    }
    return netForce;
}
void physics::PhysicsBody::removeFriction (int tag) {
    forces.erase(std::remove_if(forces.begin(), forces.end(), [tag](const Force& f) {
        return f.type == FRICTION_FORCE_TYPE && tag == f.tag;
    }));
    forceUpdate = true;
}

void physics::PhysicsBody::updateForceEquations () {
    ForceEquation** equations = resolveForces(forces, mass, *x, *y, velX, velY);
    xEquation = equations[0];
    yEquation = equations[1];
    //delete equations;
}