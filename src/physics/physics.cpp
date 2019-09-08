#include "physics.h"

using namespace physics;

physics::Force::Force(float xComponent, float yComponent, int degree, int type, int tag, float time) {
    this->xComponent = xComponent;
    this->yComponent = yComponent;
    this->degree = degree;
    this->type = type;
    this->time = time;
    this->tag = tag;
}