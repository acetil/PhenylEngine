#include "graphics/maths_headers.h"
#include "graphics/camera.h"

using namespace phenyl::graphics;

Camera::Camera() {
    positionMat = glm::mat4(1.0f);
    scaleMat = glm::mat4(1.0f);
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void Camera::translate (float x, float y) {
    positionMat = glm::translate(positionMat, glm::vec3(-x, -y, 0.0f));
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void Camera::scale (float scale) {
    scaleMat = glm::scale(scaleMat, glm::vec3(scale));
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void Camera::setPosition (float x, float y) {
    positionMat = glm::translate(glm::mat4(1.0f), glm::vec3(-x, -y, 0.0f));
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void Camera::setScale (float scale) {
    scaleMat = glm::mat4(scale);
    camMatrix = scaleMat * positionMat;
}
glm::mat4 Camera::getCamMatrix () const {
    return camMatrix;
}
std::string Camera::getUniformName () {
    return "camera";
}

glm::vec2 Camera::getWorldPos2D (glm::vec2 screenPos) {
    auto worldPos4 (camMatrix * glm::vec4{screenPos, 0, 1});

    return -glm::vec2{worldPos4.x, worldPos4.y};
}

void Camera::setPos2D (glm::vec2 newPos) {
    positionMat = glm::translate(glm::mat4(1.0f), glm::vec3(-newPos, 0.0f));
    camMatrix = scaleMat * positionMat;
}

std::string_view Camera::getName () const noexcept {
    return "Camera";
}
