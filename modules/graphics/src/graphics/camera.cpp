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