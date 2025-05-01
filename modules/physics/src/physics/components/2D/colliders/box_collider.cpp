#include "core/serialization/serializer_impl.h"

#include "physics/components/2D/colliders/box_collider.h"
#include "physics/2d/collisions_2d.h"

using namespace phenyl;

namespace phenyl::physics {
    PHENYL_SERIALIZABLE(BoxCollider2D,
        PHENYL_SERIALIZABLE_INHERITS_NAMED(Collider2D, "Collider2D"),
        PHENYL_SERIALIZABLE_MEMBER(m_scale))
}

static float calculateRadius (glm::mat2 scaleMatrix) {
    auto vec = scaleMatrix * glm::vec2{1, 1};
    return glm::length(vec);
}

static inline float sqLength (glm::vec2 vec) {
    return glm::dot(vec, vec);
}

void physics::BoxCollider2D::applyFrameTransform (glm::mat2 transform) {
    m_frameTransform = transform * glm::mat2{{m_scale.x, 0.0f}, {0.0f, m_scale.y}};

    setOuterRadius(calculateRadius(m_frameTransform));
}

static util::Optional<float> testAxisNew (glm::vec2 axis, glm::vec2 disp, float box1Axis, const glm::mat2& box2Mat) {
    PHENYL_DASSERT(box1Axis >= 0);

    float minAxis = std::numeric_limits<float>::max();
    float maxAxis = -std::numeric_limits<float>::max();

    glm::vec2 boxPoints[] = {
            {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };

    for (auto i : boxPoints) {
        glm::vec2 boxVec = box2Mat * i + disp;
        float projVal = glm::dot(axis, boxVec);

        minAxis = glm::min(minAxis, projVal);
        maxAxis = glm::max(maxAxis, projVal);
    }

    if (minAxis >= box1Axis || maxAxis <= -box1Axis) {
        return util::NullOpt;
    } else {
        auto minDisp = box1Axis - minAxis;
        auto maxDisp = (-box1Axis) - maxAxis;

        return minDisp <= -maxDisp ? util::Optional{minDisp} : util::Optional{maxDisp};
    }
}

util::Optional<physics::SATResult2D> physics::BoxCollider2D::collide (const physics::BoxCollider2D& other) {
    auto disp = getDisplacement(other);

    glm::vec2 basisVecs[] = {
            {1, 0}, {0, 1}
    };

    float minSep = std::numeric_limits<float>::max();
    glm::vec2 minAxis = {0, 0};
    float minSepSqSize = std::numeric_limits<float>::max();

    for (auto i : basisVecs) {
        auto scaledAxis = m_frameTransform * i;
        auto normAxis = glm::normalize(scaledAxis);
        auto axisOpt = testAxisNew(normAxis, disp, glm::dot(scaledAxis, normAxis), other.m_frameTransform);

        if (!axisOpt) {
            return util::NullOpt;
        }

        float sep = axisOpt.getUnsafe();
        auto sqSepSize = sep * sep;
        if (sqSepSize < minSepSqSize) {
            minSep = sep;
            minAxis = normAxis;
            minSepSqSize = sqSepSize;
        }
    }

    for (auto i : basisVecs) {
        auto scaledAxis = other.m_frameTransform * i;
        auto normAxis = glm::normalize(scaledAxis);
        auto axisOpt = testAxisNew(normAxis, -disp, glm::dot(scaledAxis, normAxis), m_frameTransform);

        if (!axisOpt) {
            return util::NullOpt;
        }

        float sep = axisOpt.getUnsafe();
        auto sqSepSize = sep * sep;
        if (sqSepSize < minSepSqSize) {
            minSep = sep;
            minAxis = -normAxis;

            minSepSqSize = sqSepSize;
        }
    }

    if (minSep < 0) {
        minSep *= -1;
        minAxis *= -1;
    }

    return std::abs(minSepSqSize) > std::numeric_limits<float>::epsilon() ? util::Optional{SATResult2D{.normal=minAxis, .depth=minSep}} : util::NullOpt;
}

static inline glm::vec2 calcSegmentNormal (glm::vec2 start, glm::vec2 end) {
    auto dv = end - start;

    return glm::normalize(glm::vec2{-dv.y, dv.x});
}

physics::Face2D physics::BoxCollider2D::getSignificantFace (glm::vec2 normal) {
    glm::vec2 boxPoints[] = {
            {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
    };

    glm::vec2 furthestVertex = {0, 0};
    float furthestDistance = -std::numeric_limits<float>::max();
    unsigned int pointIndex = -1;
    for (auto i = 0; i < sizeof(boxPoints) / sizeof(glm::vec2); i++) {
        auto vertex = m_frameTransform * boxPoints[i];
        auto dist = glm::dot(vertex, normal);
        if (dist > furthestDistance) {
            furthestDistance = dist;
            furthestVertex = vertex;
            pointIndex = i;
        }
    }

    auto vec1 = m_frameTransform * boxPoints[(pointIndex + 1) % (sizeof(boxPoints) / sizeof(glm::vec2))];
    auto vec2 = m_frameTransform * boxPoints[(pointIndex + (sizeof(boxPoints) / sizeof(glm::vec2)) - 1) % (sizeof(boxPoints) / sizeof(glm::vec2))];

    auto norm1 = -calcSegmentNormal(furthestVertex, vec1);
    auto norm2 = -calcSegmentNormal(vec2, furthestVertex);

    auto dot1 = glm::dot(norm1, normal);
    auto dot2 = glm::dot(norm2, normal);

    //PHENYL_DASSERT(dot1 >= 0);
    //PHENYL_DASSERT(dot2 >= 0);

    if (dot1 < 0) {
        norm1 *= -1;
        dot1 *= -1;
    }

    if (dot2 < 0) {
        norm2 *= -1;
        dot2 *= -1;
    }

    return dot1 >= dot2 ? Face2D{.vertices={furthestVertex + getPosition(), vec1 + getPosition()}, .normal=norm1} : Face2D{.vertices={vec2 + getPosition(), furthestVertex + getPosition()}, .normal=norm2};

}