#include "core/maths/2d/transform.h"

#include "core/serialization/serializer_impl.h"

using namespace phenyl::core;

namespace phenyl::core {
PHENYL_SERIALIZABLE(Transform2D, PHENYL_SERIALIZABLE_MEMBER_NAMED(m_position, "position"),
    PHENYL_SERIALIZABLE_METHOD("rotation", &Transform2D::rotationAngle, &Transform2D::setRotation),
    PHENYL_SERIALIZABLE_MEMBER_NAMED(m_scale, "scale"));
}

static inline glm::vec2 rotationCompose (glm::vec2 cRot1, glm::vec2 cRot2) {
    return {cRot1.x * cRot2.x - cRot1.y * cRot2.y, cRot1.x * cRot2.y + cRot1.y * cRot2.x};
}

static inline glm::vec2 rotationFromAngle (float rad) {
    return glm::vec2{glm::cos(rad), glm::sin(rad)};
}

float Transform2D::rotationAngle () const {
    return glm::atan(m_complexRotation.y, m_complexRotation.x);
}

Transform2D::Transform2D (glm::vec2 posVec, glm::vec2 scaleVec, glm::vec2 rotVec) :
    m_position{posVec},
    m_scale{scaleVec},
    m_complexRotation{rotVec} {}

Transform2D::operator AffineTransform2D () const noexcept {
    auto scaleRotMat = getMatrix();
    auto off = position();
    glm::mat3 mat = {{scaleRotMat[0][0], scaleRotMat[0][1], 0}, {scaleRotMat[1][0], scaleRotMat[1][1], 0},
      {off.x, off.y, 1.0f}};

    return AffineTransform2D{mat};
}

Transform2D& Transform2D::translate (glm::vec2 deltaPos) {
    m_position += deltaPos;
    return *this;
}

Transform2D Transform2D::withTranslation (glm::vec2 deltaPos) const {
    return Transform2D{m_position + deltaPos, m_scale, m_complexRotation};
}

Transform2D& Transform2D::setPosition (glm::vec2 newPosition) {
    m_position = newPosition;
    return *this;
}

Transform2D Transform2D::withPosition (glm::vec2 newPosition) const {
    return Transform2D{newPosition, m_scale, m_complexRotation};
}

Transform2D& Transform2D::scaleBy (glm::vec2 deltaScale) {
    m_scale *= deltaScale;
    return *this;
}

Transform2D Transform2D::withScaleBy (glm::vec2 deltaScale) const {
    return Transform2D{m_position, m_scale * deltaScale, m_complexRotation};
}

Transform2D& Transform2D::setScale (glm::vec2 newScale) {
    m_scale = newScale;
    return *this;
}

Transform2D Transform2D::withScale (glm::vec2 newScale) const {
    return Transform2D{m_position, newScale, m_complexRotation};
}

Transform2D& Transform2D::rotateBy (float deltaAngle) {
    m_complexRotation = rotationCompose(m_complexRotation, rotationFromAngle(deltaAngle));
    return *this;
}

Transform2D Transform2D::withRotateBy (float deltaAngle) const {
    return Transform2D{m_position, m_scale, rotationCompose(m_complexRotation, rotationFromAngle(deltaAngle))};
}

Transform2D& Transform2D::setRotation (float angle) {
    m_complexRotation = rotationFromAngle(angle);
    return *this;
}

Transform2D Transform2D::withRotation (float angle) {
    return Transform2D{m_position, m_scale, rotationFromAngle(angle)};
}
