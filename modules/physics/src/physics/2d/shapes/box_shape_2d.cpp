#include "box_shape_2d.h"
#include "physics/shape/2d/box_shape_2d_interface.h"

using namespace physics;

static_assert(good_shape_2d<BoxShape2D>);

static float calculateRadius (glm::mat2 scaleMatrix) {
    auto vec = scaleMatrix * glm::vec2{1, 1};
    return glm::length(vec);
}

static inline float sqLength (glm::vec2 vec) {
    return glm::dot(vec, vec);
}

BoxShape2D::BoxShape2D (ColliderId collider, glm::vec2 scale, std::uint64_t layers, std::uint64_t mask) : Shape2D{collider, calculateRadius({{scale.x, 0}, {0, scale.y}}), layers, mask},
                                                                                                          scaleMatrix{{scale.x, 0}, {0, scale.y}}, frameTransform{{scale.x, 0}, {0, scale.y}}, invFrameTransform{glm::inverse(glm::mat2{{scale.x, 0}, {0, scale.y}})} {
    setOuterRadius(calculateRadius(scaleMatrix));
}

BoxShape2D::BoxShape2D (ColliderId collider, glm::mat2 scaleMat, std::uint64_t layers, std::uint64_t mask) : Shape2D{collider, calculateRadius(scaleMat), layers, mask},
                                                                                                             scaleMatrix{scaleMat}, frameTransform{scaleMat}, invFrameTransform{glm::inverse(scaleMat)} {}

glm::vec2 BoxShape2D::getScale () const {
    return {scaleMatrix[0].x, scaleMatrix[1].y};
}

void BoxShape2D::setScale (glm::vec2 scale) {
    scaleMatrix = {{scale.x, 0}, {0, scale.y}};
}

void BoxShape2D::applyTransform (glm::mat2 transform) {
    frameTransform = transform * scaleMatrix;
    invFrameTransform = glm::inverse(frameTransform);

    setOuterRadius(calculateRadius(frameTransform));
}

util::Optional<float> testAxisNew (glm::vec2 axis, glm::vec2 disp, float box1Axis, const glm::mat2& box2Mat) {
    assert(box1Axis >= 0);

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

util::Optional<SATResult2D> BoxShape2D::collide (const physics::BoxShape2D& other) {
    auto disp = getDisplacement(other);

    glm::vec2 basisVecs[] = {
            {1, 0}, {0, 1}
    };

    float minSep = std::numeric_limits<float>::max();
    glm::vec2 minAxis = {0, 0};
    float minSepSqSize = std::numeric_limits<float>::max();

    for (auto i : basisVecs) {
        auto scaledAxis = frameTransform * i;
        auto normAxis = glm::normalize(scaledAxis);
        auto axisOpt = testAxisNew(normAxis, disp, glm::dot(scaledAxis, normAxis), other.frameTransform);

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
        auto scaledAxis = other.frameTransform * i;
        auto normAxis = glm::normalize(scaledAxis);
        auto axisOpt = testAxisNew(normAxis, -disp, glm::dot(scaledAxis, normAxis), frameTransform);

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

    return std::abs(minSepSqSize) > std::numeric_limits<float>::epsilon() ? util::Optional{SATResult2D{.normal=minAxis, .depth=minSep}} : util::NullOpt;
}

util::DataValue BoxShape2D::serialise () const {
    util::DataObject obj;
    obj["type"] = util::DataValue{"BoxShape2D"};
    obj["scale_matrix"] = scaleMatrix;
    return util::DataValue{obj};
}

util::Optional<BoxShape2D> BoxShape2D::deserialise (const util::DataObject& obj, ColliderId collider, std::uint64_t layers, std::uint64_t mask) {
    if (!obj.contains("scale_matrix")) {
        return util::NullOpt;
    }

    glm::mat2 mat;
    if (!phenyl_from_data(obj.at("scale_matrix"), mat)) {
        return util::NullOpt;
    }

    return {BoxShape2D{collider, mat, layers, mask}};
}

glm::mat2 BoxShape2D::getTransform () const {
    return frameTransform;
}

BoxShape2D ShapeRequest<BoxShape2D>::make (physics::ColliderId collider) {
    return BoxShape2D{collider, scale, layers, mask};
}
