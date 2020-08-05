#include "maths_headers.h"
#include "camera.h"

using namespace graphics;

graphics::Camera::Camera() {
    positionMat = glm::mat4(1.0f);
    scaleMat = glm::mat4(1.0f);
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void graphics::Camera::translate (float x, float y) {
    positionMat = glm::translate(positionMat, glm::vec3(x, y, 0.0f));
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void graphics::Camera::scale (float scale) {
    scaleMat = glm::scale(scaleMat, glm::vec3(scale));
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void graphics::Camera::setPosition (float x, float y) {
    positionMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    camMatrix = scaleMat * positionMat;
}

[[maybe_unused]] void graphics::Camera::setScale (float scale) {
    scaleMat = glm::mat4(scale);
    camMatrix = scaleMat * positionMat;
}
glm::mat4 graphics::Camera::getCamMatrix () {
    return camMatrix;
}
std::string graphics::Camera::getUniformName () {
    return "camera";
}