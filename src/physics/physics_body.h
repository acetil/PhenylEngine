#include <vector>
#include "physics.h"
#include "physics_equations.h"
#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#define CONST_V_EQ 0
#define LINEAR_V_EQ 1

namespace physics {
    class PhysicsBody {
        private:
        float* x;
        float* y;
        float velX;
        float velY;
        float mass;
        float constFrictionK;
        float linearFrictionK;
        std::vector<Force> forces;
        ForceEquation* xEquation;
        ForceEquation* yEquation;
        bool forceUpdate;
        public:
        void updatePosition (float addedTime);
        void addForce (Force f);
        void removeMovementForces ();
        void setVelocity (float velX, float velY);

    };
}
#endif