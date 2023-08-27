#include "collisions_2d.h"

#include "logging/logging.h"

using namespace physics;

// Solve x1 + u1t1 = x2 + u2t2 and y1 + v1t1 = y2 + v2t2 by eliminating t2
// You get t1 = (v2(x2 - x1) - u2(y2 - y1)) / (u1v2 - u2v1)
static inline float lineIntersection (glm::vec2 p, glm::vec2 dv, glm::vec2 ip, glm::vec2 idv) {
    float u1 = dv.x;
    float u2 = idv.x;
    float v1 = dv.y;
    float v2 = idv.y;

    return (v2 * (ip.x - p.x) - u2 * (ip.y - p.y)) / (u1 * v2 - u2 * v1);
}

static inline Manifold2D buildManifoldInternal (const physics::Face2D& refFace, const physics::Face2D& incFace, glm::vec2 normal, float depth) {
    auto dv = incFace.vertices[1] - incFace.vertices[0];
    //logging::log(LEVEL_DEBUG, "Intersect0 = {}", lineIntersection(incFace.vertices[0], dv, refFace.vertices[0], refFace.normal));
    //logging::log(LEVEL_DEBUG, "Intersect1 = {}", lineIntersection(incFace.vertices[0], dv, refFace.vertices[1], refFace.normal));
    auto inc1 = glm::clamp(lineIntersection(incFace.vertices[0], dv, refFace.vertices[0], refFace.normal), 0.0f, 1.0f) * dv + incFace.vertices[0];
    auto inc2 = glm::clamp(lineIntersection(incFace.vertices[0], dv, refFace.vertices[1], refFace.normal), 0.0f, 1.0f) * dv + incFace.vertices[0];



    //logging::log(LEVEL_DEBUG, "Clipped points: <{}, {}>, <{}, {}>", inc1.x, inc1.y, inc2.x, inc2.y);

    // At least one point should not be clipped
    assert(glm::dot(inc1 - refFace.vertices[0], refFace.normal) <= 0 || glm::dot(inc2 - refFace.vertices[0], refFace.normal) <= 0);

    if (glm::dot(inc1 - refFace.vertices[0], refFace.normal) > 0) {
        return Manifold2D{.points={inc2, inc2}, .normal=normal, .depth=depth, .type=Manifold2DType::POINT};
    } else if (glm::dot(inc2 - refFace.vertices[0], refFace.normal) > 0) {
        return Manifold2D{.points={inc1, inc1}, .normal=normal, .depth=depth, .type=Manifold2DType::POINT};
    } else {
        return Manifold2D{.points={inc1, inc2}, .normal=normal, .depth=depth, .type=Manifold2DType::LINE};
    }
}

Manifold2D physics::buildManifold (const physics::Face2D& face1, const physics::Face2D& face2, glm::vec2 normal, float depth) {
    if (glm::dot(face1.normal, normal) >= glm::dot(face2.normal, -normal)) {
        //logging::log(LEVEL_DEBUG, "Ref face: face1, Inc face: face2");
        return buildManifoldInternal(face1, face2, normal, depth);
    } else {
        //logging::log(LEVEL_DEBUG, "Ref face: face2, Inc face: face1");
        return buildManifoldInternal(face2, face1, normal, depth);
    }
}