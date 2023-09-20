#pragma once

#include "rigid_body.h"
#include "common/serializer.h"
#include "common/maths/glm_serializer.h"

namespace phenyl::physics {
    PHENYL_SERIALIZE(RigidBody2D, {
        PHENYL_MEMBER(momentum);
        PHENYL_MEMBER_NAMED(angularMomentum, "angular_momentum");
        PHENYL_MEMBER_METHOD("mass", getMass, setMass);
        PHENYL_MEMBER_METHOD("inertial_moment", getInertia, setInertia);
        PHENYL_MEMBER(drag);
        PHENYL_MEMBER_NAMED(angularDrag, "angular_drag");
        PHENYL_MEMBER(gravity);
    })
}