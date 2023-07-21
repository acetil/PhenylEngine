#include "common/maths/2d/transform.h"
#include "util/data.h"

using namespace common;

static inline glm::vec2 rotationCompose (glm::vec2 cRot1, glm::vec2 cRot2) {
    return {cRot1.x * cRot2.x - cRot1.y * cRot2.y, cRot1.x * cRot2.y + cRot1.y * cRot2.x};
}

static inline glm::vec2 rotationFromAngle (float rad) {
    return glm::vec2{glm::cos(rad), glm::sin(rad)};
}

glm::vec2 common::Transform2D::position () const {
    return positionVec;
}

glm::vec2 Transform2D::scale () const {
    return scaleVec;
}

float Transform2D::rotationAngle () const {
    return glm::atan(complexRotation.y, complexRotation.x);
}

Transform2D::Transform2D (glm::vec2 posVec, glm::vec2 scaleVec, glm::vec2 rotVec) : positionVec{posVec}, scaleVec{scaleVec}, complexRotation{rotVec} {}


glm::mat2 Transform2D::scaleMatrix () const {
    return {{scaleVec.x, 0}, {0, scaleVec.y}};
}

glm::mat2 Transform2D::rotMatrix () const {
    return {{complexRotation.x, complexRotation.y}, {-complexRotation.y, complexRotation.x}};
}

glm::mat2 Transform2D::getMatrix () const {
    return scaleMatrix() * rotMatrix();
}

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

bool common::phenyl_from_data (const util::DataValue& val, common::Transform2D& transform) {
    if (!val.is<util::DataObject>()) {
        return false;
    }
    const auto& obj = val.get<util::DataObject>();

    if (!obj.contains("position")) {
        return false;
    }
    const auto& posData = obj.at("position");
    if (!phenyl_from_data(posData, transform.positionVec)) {
        return false;
    }

    if (!obj.contains("scale")) {
        return false;
    }
    const auto& scaleData = obj.at("scale");
    if (!phenyl_from_data(scaleData, transform.scaleVec)) {
        return false;
    }

    if (!obj.contains("rotation")) {
        return false;
    }
    const auto& rotation = obj.at("rotation");
    if (!rotation.is<float>()) {
        return false;
    }

    transform.complexRotation = rotationFromAngle(rotation.get<float>());

    return true;
}

util::DataValue common::phenyl_to_data (const common::Transform2D& transform) {
    util::DataObject obj;

    obj["position"] = transform.positionVec;
    obj["scale"] = transform.scaleVec;
    obj["rotation"] = transform.rotationAngle();

    return obj;
}