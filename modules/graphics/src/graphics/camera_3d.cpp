#include "graphics/camera_3d.h"

using namespace phenyl::graphics;

Camera3D::Camera3D (glm::vec2 resolution) : resolution{resolution} {}

void Camera3D::lookAt (glm::vec3 target, glm::vec3 upAxis) noexcept {
    if (target != transform.position()) {
        transform.setRotation(core::Quaternion::LookAt(target - transform.position(), upAxis));
    }
}

glm::mat4 Camera3D::view () const noexcept {
    auto pos = transform.position();
    auto rotation = transform.rotation();

    glm::mat4 translationMatrix{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {-pos.x, -pos.y, -pos.z, 1}
    };
    return static_cast<glm::mat4>(rotation.inverse()) * translationMatrix;
}

glm::mat4 Camera3D::projection () const noexcept {
    return glm::perspective(glm::radians(fov), resolution.x / resolution.y, nearClippingPlane, farClippingPlane);
}

glm::mat4 Camera3D::matrix () const noexcept {
    return projection() * view();
}

void Camera3D::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    resolution = newResolution;
}

std::string_view Camera3D::getName () const noexcept {
    return "Camera3D";
}
