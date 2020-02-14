#ifndef MAIN_COMPONENT_H
#define MAIN_COMPONENT_H
namespace component {
    struct EntityMainComponent {
        float pos[2];
        float vel[2];
        float acc[2];
        float constFriction;
        float linFriction;
        float vec1[2];
        float vec2[2];
    };
}
#endif