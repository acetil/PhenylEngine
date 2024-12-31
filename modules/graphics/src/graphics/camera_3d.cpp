#include "graphics/camera_3d.h"

using namespace phenyl::graphics;

Camera3D::Camera3D (glm::vec2 resolution) : resolution{resolution} {}

void Camera3D::lookAt (glm::vec3 target, glm::vec3 upAxis) noexcept {
    if (target != transform.position()) {
        transform.setRotation(core::Quaternion::LookAt(target - transform.position(), upAxis));
    }
}

glm::mat4 Camera3D::viewMatrix () const noexcept {
    auto pos = transform.position();
    auto rotation = transform.rotation();

    glm::mat4 translationMatrix{
        {0, 0, 0, -pos.x},
        {0, 0, 0, -pos.y},
        {0, 0, 0, -pos.z},
        {0, 0, 0, 1}
    };
    return static_cast<glm::mat4>(rotation.inverse()) * translationMatrix;
}

glm::mat4 Camera3D::perspectiveMatrix () const noexcept {
    return glm::perspective(glm::radians(fov), resolution.y / resolution.x, nearClippingPlane, farClippingPlane);
}

glm::mat4 Camera3D::cameraMatrix () const noexcept {
    return perspectiveMatrix() * viewMatrix();
}

void Camera3D::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    resolution = newResolution;
}

std::string_view Camera3D::getName () const noexcept {
    return "Camera3D";
}
