#include <vector>

#include "physics.h"
#ifndef PHYSICS_EQUATIONS_H
#define PHYSICS_EQUATIONS_H
namespace physics {
    class ForceEquation {
        protected:
        float x0;
        float v0;
        float a;
        float b;
        float addedTime;
        public:
        virtual void setFactors (float a, float b, float x0, float v0);
        virtual float getVelocity (float t) = 0;
        virtual float getPosition (float t) = 0;
        virtual float getPosIntercept (float intercept) = 0;
        virtual float getVelocityIntercept (float intercept) = 0;
        virtual void updateTime (float t); 
    };
    ForceEquation** resolveForces (std::vector<Force> forces, float mass, float initialX, float initialY, 
        float velX, float velY);
}
#endif