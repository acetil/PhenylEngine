#include "common/maths/2d/transform.h"
#include "common/serialization/serializer_impl.h"
#include "util/data.h"

using namespace phenyl::common;

namespace phenyl::common {
    PHENYL_SERIALIZABLE(Transform2D,
        PHENYL_SERIALIZABLE_MEMBER_NAMED(positionVec, "position"),
        PHENYL_SERIALIZABLE_METHOD("rotation", &Transform2D::rotationAngle, &Transform2D::setRotation))
}

static inline glm::vec2 rotationCompose (glm::vec2 cRot1, glm::vec2 cRot2) {
    return {cRot1.x * cRot2.x - cRot1.y * cRot2.y, cRot1.x * cRot2.y + cRot1.y * cRot2.x};
}

static inline glm::vec2 rotationFromAngle (float rad) {
    return glm::vec2{glm::cos(rad), glm::sin(rad)};
}

float Transform2D::rotationAngle () const {
    return glm::atan(complexRotation.y, complexRotation.x);
}

Transform2D::Transform2D (glm::vec2 posVec, glm::vec2 scaleVec, glm::vec2 rotVec) : positionVec{posVec}, scaleVec{scaleVec}, complexRotation{rotVec} {}

Transform2D& Transform2D::translate (glm::vec2 deltaPos) {
    positionVec += deltaPos;
    return *this;
}

Transform2D Transform2D::withTranslation (glm::vec2 deltaPos) const {
    return Transform2D{positionVec + deltaPos, scaleVec, complexRotation};
}

Transform2D& Transform2D::setPosition (glm::vec2 newPosition) {
    positionVec = newPosition;
    return *this;
}

Transform2D Transform2D::withPosition (glm::vec2 newPosition) const {
    return Transform2D{newPosition, scaleVec, complexRotation};
}

Transform2D& Transform2D::scaleBy (glm::vec2 deltaScale) {
    scaleVec *= deltaScale;
    return *this;
}

Transform2D Transform2D::withScaleBy (glm::vec2 deltaScale) const {
    return Transform2D{positionVec, scaleVec * deltaScale, complexRotation};
}

Transform2D& Transform2D::setScale (glm::vec2 newScale) {
    scaleVec = newScale;
    return *this;
}

Transform2D Transform2D::withScale (glm::vec2 newScale) const {
    return Transform2D{positionVec, newScale, complexRotation};
}

Transform2D& Transform2D::rotateBy (float deltaAngle) {
    complexRotation = rotationCompose(complexRotation, rotationFromAngle(deltaAngle));
    return *this;
}

Transform2D Transform2D::withRotateBy (float deltaAngle) const {
    return Transform2D{positionVec, scaleVec, rotationCompose(complexRotation, rotationFromAngle(deltaAngle))};
}

Transform2D& Transform2D::setRotation (float angle) {
    complexRotation = rotationFromAngle(angle);
    return *this;
}

Transform2D Transform2D::withRotation (float angle) {
    return Transform2D{positionVec, scaleVec, rotationFromAngle(angle)};
}