#include "collisions_2d.h"

#include "logging/logging.h"

#define BAUMGARTE_TERM 0.2f
#define BAUMGARTE_SLOP (-0.001f)

inline float vec2dCross (glm::vec2 vec1, glm::vec2 vec2) {
    return vec1.x * vec2.y - vec1.y * vec2.x;
}

using namespace phenyl::physics;

// Solve x1 + u1t1 = x2 + u2t2 and y1 + v1t1 = y2 + v2t2 by eliminating t2
// You get t1 = (v2(x2 - x1) - u2(y2 - y1)) / (u1v2 - u2v1)
static inline float lineIntersection (glm::vec2 p, glm::vec2 dv, glm::vec2 ip, glm::vec2 idv) {
    float u1 = dv.x;
    float u2 = idv.x;
    float v1 = dv.y;
    float v2 = idv.y;

    return (v2 * (ip.x - p.x) - u2 * (ip.y - p.y)) / (u1 * v2 - u2 * v1);
}

static inline Manifold2D buildManifoldInternal (const Face2D& refFace, const Face2D& incFace, glm::vec2 normal, float depth) {
    auto dv = incFace.vertices[1] - incFace.vertices[0];
    auto inc1 = glm::clamp(lineIntersection(incFace.vertices[0], dv, refFace.vertices[0], refFace.normal), 0.0f, 1.0f) * dv + incFace.vertices[0];
    auto inc2 = glm::clamp(lineIntersection(incFace.vertices[0], dv, refFace.vertices[1], refFace.normal), 0.0f, 1.0f) * dv + incFace.vertices[0];

    // At least one point should not be clipped
    PHENYL_DASSERT(glm::dot(inc1 - refFace.vertices[0], refFace.normal) <= 0 || glm::dot(inc2 - refFace.vertices[0], refFace.normal) <= 0);

    if (glm::dot(inc1 - refFace.vertices[0], refFace.normal) > 0) {
        return Manifold2D{.points={inc2, inc2}, .normal=normal, .depth=depth, .type=Manifold2DType::POINT};
    } else if (glm::dot(inc2 - refFace.vertices[0], refFace.normal) > 0) {
        return Manifold2D{.points={inc1, inc1}, .normal=normal, .depth=depth, .type=Manifold2DType::POINT};
    } else {
        return Manifold2D{.points={inc1, inc2}, .normal=normal, .depth=depth, .type=Manifold2DType::LINE};
    }
}

Manifold2D phenyl::physics::buildManifold (const physics::Face2D& face1, const physics::Face2D& face2, glm::vec2 normal, float depth) {
    if (glm::dot(face1.normal, normal) >= glm::dot(face2.normal, -normal)) {
        return buildManifoldInternal(face1, face2, normal, depth);
    } else {
        return buildManifoldInternal(face2, face1, normal, depth);
    }
}

Constraint2D Manifold2D::buildConstraint (Collider2D* obj1, Collider2D* obj2, float deltaTime) const {
    auto contactPoint = (points[0] + points[1]) / 2.0f;
    auto r1 = contactPoint - obj1->getPosition();
    auto r2 = contactPoint - obj2->getPosition();

    auto elasticity = obj1->elasticity * obj2->elasticity; // TODO: different types of resolution?
    auto elasticityTerm = glm::dot(normal, obj1->momentum * obj1->invMass + r1 * obj1->angularMomentum * obj1->invInertiaMoment - obj2->momentum * obj2->invMass - r2 * obj2->angularMomentum * obj2->invInertiaMoment);

    float bias = -BAUMGARTE_TERM / deltaTime * (glm::max(depth + BAUMGARTE_SLOP, 0.0f)) - elasticity * elasticityTerm;
    return Constraint2D::ContactConstraint(obj1, obj2, contactPoint, normal, bias);
}

glm::vec2 Manifold2D::getContactPoint () const {
    return (points[0] + points[1]) / 2.0f;
}

Constraint2D Constraint2D::ContactConstraint (Collider2D* obj1, Collider2D* obj2, glm::vec2 contactPoint, glm::vec2 normal, float bias) {
    // See https://kevinyu.net/2018/01/17/understanding-constraint-solver-in-physics-engine/ and
    // https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics6collisionresponse/2017%20Tutorial%206%20-%20Collision%20Response.pdf

    auto r1 = contactPoint - obj1->currentPos;
    auto r2 = contactPoint - obj2->currentPos;

    auto jVelObj1 = obj1->invMass != 0 ? -normal : glm::vec2{0, 0};
    auto jWObj1 = obj1->invInertiaMoment != 0 ? vec2dCross(-r1, normal) : 0.0f;

    auto jVelObj2 = obj2->invMass != 0 ? normal : glm::vec2{0, 0};
    auto jWObj2 = obj2->invInertiaMoment != 0 ? vec2dCross(r2, normal) : 0.0f;

    float jacobMass = 0.0f;
    jacobMass += glm::dot(jVelObj1 * glm::vec2{1 * obj1->invMass, 1 * obj1->invMass}, jVelObj1);
    jacobMass += glm::dot(jVelObj2 * glm::vec2{1 * obj2->invMass, 1 * obj2->invMass}, jVelObj2);
    jacobMass += jWObj1 * obj1->invInertiaMoment * jWObj1;
    jacobMass += jWObj2 * obj2->invInertiaMoment * jWObj2;

    auto invJacobMass = 1 / jacobMass;

    return Constraint2D {
            .obj1=obj1,
            .obj2=obj2,
            .jVelObj1=jVelObj1,
            .jVelObj2=jVelObj2,
            .jWObj1=jWObj1,
            .jWObj2=jWObj2,
            .invJacobMass=invJacobMass,
            .bias=bias,
            .lambdaClamp={0.0f, std::numeric_limits<float>::max()}
    };
}



bool Constraint2D::solve () {
    float lambda = -(glm::dot(jVelObj1, obj1->getCurrVelocity()) + glm::dot(jVelObj2, obj2->getCurrVelocity()) + jWObj1 * obj1->getCurrAngularVelocity() + jWObj2 * obj2->getCurrAngularVelocity() + bias) * invJacobMass;
    float newLambda = glm::clamp(lambdaSum + lambda, lambdaClamp[0], lambdaClamp[1]);

    float lambdaDiff = newLambda - lambdaSum;
    lambdaSum = newLambda;
    if (glm::abs(lambdaDiff) < std::numeric_limits<float>::epsilon()) {
        return false;
    } else {
        obj1->applyImpulse(jVelObj1 * lambdaDiff);
        obj2->applyImpulse(jVelObj2 * lambdaDiff);

        obj1->applyAngularImpulse(jWObj1 * lambdaDiff);
        obj2->applyAngularImpulse(jWObj2 * lambdaDiff);

        return true;
    }
}