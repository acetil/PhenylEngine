#ifndef MAIN_COMPONENT_H
#define MAIN_COMPONENT_H
namespace component {
    struct EntityMainComponent {
        float pos[2];
        float vel[2];
        float acc[2];
        float constFriction;
        float linFriction;
        int width;
        int height;
        float sinAngle;
        float cosAngle;
    };
}
#endif