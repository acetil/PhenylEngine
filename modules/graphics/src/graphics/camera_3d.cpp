#include "graphics/camera_3d.h"

#include "graphics/detail/loggers.h"

using namespace phenyl::graphics;

Camera3D::Camera3D (glm::vec2 resolution) : m_resolution{resolution} {}

void Camera3D::lookAt (glm::vec3 target, glm::vec3 upAxis) noexcept {
    if (target != transform.position()) {
        auto disp = target - transform.position();
        transform.setRotation(core::Quaternion::LookAt(-disp, upAxis));
    }
}

glm::mat4 Camera3D::view () const noexcept {
    auto pos = transform.position();
    auto rotation = transform.rotation().inverse();
    auto rotationMat = static_cast<glm::mat4>(rotation);

    glm::mat4 translationMatrix{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {-pos.x, -pos.y, -pos.z, 1}};
    glm::mat4 fixRotation = {{1, 0, 0, 0}, {0, -1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
    auto result = fixRotation * rotationMat * translationMatrix;
    return result;
    // return static_cast<glm::mat4>(rotation) * translationMatrix;
}

glm::mat4 Camera3D::projection () const noexcept {
    auto mat =
        glm::perspective(glm::radians(fov), m_resolution.x / m_resolution.y, nearClippingPlane, farClippingPlane);
    return mat;
    // return glm::perspective(glm::radians(fov), resolution.x / resolution.y,
    // nearClippingPlane, farClippingPlane); return glm::identity<glm::mat4>();
}

glm::mat4 Camera3D::matrix () const noexcept {
    return projection() * view();
}

void Camera3D::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    m_resolution = newResolution;
}

std::string_view Camera3D::getName () const noexcept {
    return "Camera3D";
}
