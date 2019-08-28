#include <math.h>

#include "physics_body.h"

using namespace physics;
class ConstantEquation : ForceEquation {
    // equation with no term of v (a)
    // equation: x'' = a
    public:
    void setFactors (float a, float x0, float v0);
    void setFactors (float a, float b, float x0, float v0);
    float virtual getVelocity (float t);
    float virtual getPosition (float t);
    float virtual getPosIntercept (float intercept);
    float virtual getVelocityIntercept (float intercept);
};
class LinearEquation : ForceEquation {
    // equation with term of v (a + bv)
    // equation: x'' = a + bv
    // equation: x(t) = 1/b^2 * ((e^(bt) - 1)(a + bv0) + x0b^2)
    public:
    float virtual getVelocity (float t);
    float virtual getPosition (float t);
    float virtual getPosIntercept (float intercept);
    float virtual getVelocityIntercept (float intercept);
};
class QuadEquation: ForceEquation {
    // equation with term of v (a + (bv + c)^2)
    protected:
    float c;
    public:
    void virtual setFactors (float a, float b, float c, float x0, float v0);
    void virtual setFactors (float a, float b, float x0, float v0);
    
};
void physics::ForceEquation::setFactors (float a, float b, float x0, float v0) {
    this->a = a;
    this->b = b;
    this->x0 = x0;
    this->v0 = v0;
}
void physics::ForceEquation::updateTime (float t) {
    addedTime += t;
}

void ConstantEquation::setFactors(float a, float x0, float v0) {
    setFactors(a, 0, x0, v0);    
}
float ConstantEquation::getPosition (float t) {
    // equation: x(t) = v0t + 1/2at^2 + x0
    float time = addedTime + t;
    return v0 * time + a * time * time / 2 + x0;
}
float ConstantEquation::getVelocity (float t) {
    float time = addedTime + t;
    //equation: v(t) = at + v0
    return v0 + a * time;
}
float ConstantEquation::getPosIntercept (float intercept) {
    //solve equation 0 = v0t + 1/2at^2 + x0 - x
    // we want the first intercept after current time
    // if return is negative, intercept is impossible
    float delta = v0 * v0 - 4 * (a / 2) * (x0 - intercept);
    if (delta < 0) {
        return -1;
    }
    float rootDelta = sqrt(delta);
    return (-1 * v0 - rootDelta) / a - addedTime < 0 ? (-1 * v0 - rootDelta) / a - addedTime : 
    (-1 * v0 + rootDelta) / a - addedTime; 
}
float ConstantEquation::getVelocityIntercept (float intercept) {
    // solve equation v = v0 + at
    return (intercept - v0) / a - addedTime;
}
float LinearEquation::getPosition (float t) {
    // equation: x(t) = 1/b^2 * ((e^(bt) - 1)(a + bv0) + x0b^2)
    float time = t + addedTime;
    return ((exp(time * b) - 1) * (a + b * v0) + x0 * b * b) / (b * b);
}
float LinearEquation::getVelocity (float t) {
    // equation: v(t) = 1 / b * ((a + bv0)e^(bt) - a)
    return ((a + b * v0) * exp((t + addedTime) * b)) / b;
}
float LinearEquation::getVelocityIntercept (float intercept) {
    // t = 1/b*ln((a+bv)/(a+bv0))
    return log((a + b * intercept) / (a + b * v0)) / b;
}
float LinearEquation::getPosIntercept (float intercept) {
    // t = 1/b * ln(1 + (x - x0)b^2 /(a + bv0))
    return log(1 + (intercept - x0) * b * b /(a + b * v0)) / b;
}
void QuadEquation::setFactors (float a, float b, float c, float x0, float v0) {
    setFactors(a, b, x0, v0);
    this->c = c;
}
