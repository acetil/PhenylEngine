#include "physics/components/2D/collider.h"
#include "physics/components/2D/rigid_body.h"

bool physics::ColliderComp2D::shouldCollide (const physics::ColliderComp2D& other) const {
    if (!(layers & other.mask || other.layers & mask)) {
        return false;
    }

    auto displacement = getDisplacement(other);
    float sqDispLen = glm::dot(displacement, displacement);
    float radiusLen = outerRadius + other.outerRadius;

    return sqDispLen < radiusLen * radiusLen;
}

void physics::ColliderComp2D::syncUpdates (const RigidBody2D& body, glm::vec2 pos) {
    currentPos = pos;
    invMass = body.getInvMass();
    invInertiaMoment = body.getInvInertia();
    momentum = body.getMomentum();
    angularMomentum = body.getAngularMomentum();

    appliedImpulse = {0.0f, 0.0f};
    appliedAngularImpulse = 0.0f;
}

void physics::ColliderComp2D::updateBody (physics::RigidBody2D& body) const {
    body.applyImpulse(appliedImpulse);
    body.applyAngularImpulse(appliedAngularImpulse);
}

util::DataValue physics::ColliderComp2D::serialise () const {
    util::DataObject obj;
    obj["layers"] = (unsigned int)layers;
    obj["masks"] = (unsigned int)mask;
    obj["elasticity"] = (unsigned int)elasticity;

    return obj;
}

bool physics::ColliderComp2D::deserialise (const util::DataValue& val) {
    if (!val.is<util::DataObject>()) {
        return false;
    }
    const auto& obj = val.get<util::DataObject>();

    if (!obj.contains<int>("layers")) {
        return false;
    }
    if (!obj.contains<int>("masks")) {
        return false;
    }
    if (!obj.contains<float>("elasticity")) {
        return false;
    }

    layers = obj.at<unsigned int>("layers");
    mask = obj.at<unsigned int>("masks");
    elasticity = obj.at<float>("elasticity");

    return true;
}
