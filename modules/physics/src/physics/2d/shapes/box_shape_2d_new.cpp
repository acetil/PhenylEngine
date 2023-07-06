#include "box_shape_2d_new.h"

using namespace physics;

static_assert(good_shape_2d<BoxShape2DNew>);

static float calculateRadius (glm::mat2 scaleMatrix) {
    auto vec = scaleMatrix * glm::vec2{1, 1};
    return glm::length(vec);
}

BoxShape2DNew::BoxShape2DNew (ColliderId collider, glm::vec2 scale, std::uint64_t layers, std::uint64_t mask) : Shape2DNew{collider, calculateRadius({{scale.x, 0}, {0, scale.y}}), layers, mask},
        scaleMatrix{{scale.x, 0}, {0, scale.y}}, frameTransform{{scale.x, 0}, {0, scale.y}}, invFrameTransform{glm::inverse(glm::mat2{{scale.x, 0}, {0, scale.y}})} {
    setOuterRadius(calculateRadius(scaleMatrix));
}

BoxShape2DNew::BoxShape2DNew (ColliderId collider, glm::mat2 scaleMat, std::uint64_t layers, std::uint64_t mask) : Shape2DNew{collider, calculateRadius(scaleMat), layers, mask},
        scaleMatrix{scaleMat}, frameTransform{scaleMat}, invFrameTransform{glm::inverse(scaleMat)} {}

glm::vec2 BoxShape2DNew::getScale () const {
    return {scaleMatrix[0].x, scaleMatrix[1].y};
}

void BoxShape2DNew::setScale (glm::vec2 scale) {
    scaleMatrix = {{scale.x, 0}, {0, scale.y}};
}

void BoxShape2DNew::applyTransform (glm::mat2 transform) {
    frameTransform = transform * scaleMatrix;
    invFrameTransform = glm::inverse(frameTransform);

    setOuterRadius(calculateRadius(frameTransform));
}

static inline float sqDistance (glm::vec2 vec) {
    return glm::dot(vec, vec);
}

static inline glm::vec2 projectVecBox (glm::vec2 onto, glm::vec2 vec) {
    return glm::dot(onto, vec) / (sqDistance(onto)) * onto;
}

static inline glm::vec2 getAxisPoint (glm::vec2 basis, glm::vec2 point, glm::mat2 transform, glm::mat2 invTransform, glm::vec2 disp) {
    auto transformedPoint = transform * point + disp;
    auto relPoint = projectVecBox(basis, invTransform * transformedPoint);

    return relPoint;
}

static inline float getSign (glm::vec2 vec, glm::vec2 basis) {
    return glm::dot(vec, basis) >= 0 ? 1.0f : 0.0f;
}


static util::Optional<glm::vec2> testAxis (glm::vec2 basis, glm::mat2 transform, glm::mat2 invTransform, glm::vec2 disp) {
    glm::vec2 minIntersect;
    glm::vec2 maxIntersect;
    float minIntersectVecSize = std::numeric_limits<float>::max();
    float maxIntersectVecSize = std::numeric_limits<float>::min();

    float posMax = 0.0f;
    float negMax = 0.0f;

    auto boxPoints = std::array{glm::vec2{1, 1}, glm::vec2{-1, 1}, glm::vec2{1, -1}, glm::vec2{-1, 1}};

    for (auto i : boxPoints) {
        auto axisPoint = getAxisPoint(basis, i, transform, invTransform, disp);

        auto sqDist = sqDistance(axisPoint);
        if (sqDist < minIntersectVecSize) {
            minIntersect = axisPoint;
            minIntersectVecSize = sqDist;
        }

        if (sqDist > maxIntersectVecSize) {
            maxIntersect = axisPoint;
            maxIntersectVecSize = sqDist;
        }

        float mul = glm::dot(basis, axisPoint);

        posMax = std::max(posMax, mul);
        negMax = std::min(negMax, mul);
    }

    if (minIntersectVecSize <= 1.0f) {
        float sign = getSign(maxIntersect - minIntersect, basis);

        glm::vec2 overlap = basis * sign - minIntersect;

        return {transform * overlap};
    } else if (posMax >= 1.0f && negMax <= -1.0f) {
        float minDist;
        if (posMax >= -negMax) {
            minDist = posMax + 1.0f;
        } else {
            minDist = negMax - 1.0f;
        }

        return {transform * (basis * minDist)};
    } else {
        return util::NullOpt;
    }
}

util::Optional<glm::vec2> BoxShape2DNew::collide (const BoxShape2DNew& other) {
    auto displacement = getDisplacement(other);
    auto basisVecs = std::array{glm::vec2{1, 0}, glm::vec2{0, 1}};
    float leastSquaredDist = std::numeric_limits<float>::max();
    glm::vec2 smallestDispVec{};

    for (auto i : basisVecs) {
        auto result = testAxis(i, frameTransform, other.invFrameTransform, displacement);
        if (!result) {
            return util::NullOpt;
        }

        auto resultVec = result.getUnsafe();
        auto dist = sqDistance(resultVec);
        if (dist < leastSquaredDist) {
            smallestDispVec = resultVec;
            leastSquaredDist = dist;
        }
    }

    for (auto i : basisVecs) {
        auto result = testAxis(i, other.frameTransform, invFrameTransform, -displacement);
        if (!result) {
            return util::NullOpt;
        }

        auto resultVec = result.getUnsafe();
        auto dist = sqDistance(resultVec);
        if (dist < leastSquaredDist) {
            smallestDispVec = -resultVec;
            leastSquaredDist = dist;
        }
    }

    return std::abs(leastSquaredDist) > std::numeric_limits<float>::epsilon() ? util::Optional<glm::vec2>{smallestDispVec} : util::NullOpt;
}

util::DataValue BoxShape2DNew::serialise () const {
    util::DataObject obj;
    obj["type"] = util::DataValue{"BoxShape2D"};
    obj["scale_matrix"] = scaleMatrix;
    return util::DataValue{obj};
}

util::Optional<BoxShape2DNew> BoxShape2DNew::deserialise (const util::DataObject& obj, ColliderId collider, std::uint64_t layers, std::uint64_t mask) {
    if (!obj.contains("scale_matrix")) {
        return util::NullOpt;
    }

    glm::mat2 mat;
    if (!phenyl_from_data(obj.at("scale_matrix"), mat)) {
        return util::NullOpt;
    }

    return {BoxShape2DNew{collider, mat, layers, mask}};
}


