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
        void virtual setFactors (float a, float b, float x0, float v0);
        float virtual getVelocity (float t);
        float virtual getPosition (float t);
        float virtual getPosIntercept (float intercept);
        float virtual getVelocityIntercept (float intercept);
        void virtual updateTime (float t); 
    };
    ForceEquation** resolveForces (std::vector<Force> forces, float mass);
}
#endif