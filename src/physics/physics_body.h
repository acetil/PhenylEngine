#include <vector>
#include "physics.h"
#include "physics_equations.h"
#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#define CONST_V_EQ 0
#define LINEAR_V_EQ 1
#define X_COMP 0
#define Y_COMP 1

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
        float netConstForceX = 0;
        float netConstForceY = 0;
        std::vector<Force> forces;
        ForceEquation* xEquation;
        ForceEquation* yEquation;
        bool forceUpdate = true;
        float getNetForce (int direction);
        void removeFriction (int tag);
        void updateForceEquations ();
        public:
        PhysicsBody (float* x, float* y, float mass, float constFrictionK, float linearFrictionK);
        void updatePosition (float addedTime);
        void addForce (Force f);
        void removeMovementForces (int tag);
        void setVelocity (float velX, float velY);
    };
}
#endif