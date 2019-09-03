#ifndef PHYSICS_H
#define PHYSICS_H
#define CONSTANT_DEGREE 0
#define LINEAR_DEGREE 1
#define QUADRATIC_DEGREE 2
#define MOVEMENT_FORCE_TYPE 0
#define FRICTION_FORCE_TYPE 1
#define APPLIED_FORCE_TYPE 2

namespace physics {
    class Force {
        public:
        float xComponent;
        float yComponent;
        int degree;
        int type;
        float time;
        Force () {
            
        };
        Force(float xComponent, float yComponent, int degree, int type, float time);
    };
}
#endif