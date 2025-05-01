#include "graphics/maths_headers.h"
#include "graphics/camera_2d.h"

using namespace phenyl::graphics;

Camera2D::Camera2D(glm::vec2 resolution) : m_resolution{resolution} {
    m_positionMat = glm::mat4(1.0f);
    m_scaleMat = glm::mat4(1.0f);
    m_matrix = m_scaleMat * m_positionMat;
}

[[maybe_unused]] void Camera2D::translate (float x, float y) {
    m_positionMat = glm::translate(m_positionMat, glm::vec3(-x, -y, 0.0f));
    m_matrix = m_scaleMat * m_positionMat;
}

[[maybe_unused]] void Camera2D::scale (float scale) {
    m_scaleMat = glm::scale(m_scaleMat, glm::vec3(scale));
    m_matrix = m_scaleMat * m_positionMat;
}

[[maybe_unused]] void Camera2D::setPosition (float x, float y) {
    m_positionMat = glm::translate(glm::mat4(1.0f), glm::vec3(-x, -y, 0.0f));
    m_matrix = m_scaleMat * m_positionMat;
}

[[maybe_unused]] void Camera2D::setScale (float scale) {
    m_scaleMat = glm::mat4(scale);
    m_matrix = m_scaleMat * m_positionMat;
}
glm::mat4 Camera2D::getCamMatrix () const {
    return m_matrix;
}
std::string Camera2D::getUniformName () {
    return "camera";
}

glm::vec2 Camera2D::getWorldPos2D (glm::vec2 screenPos) const {
    glm::vec2 viewPos = screenPos / m_resolution * 2.0f - glm::vec2{1.0f, 1.0f};
    viewPos.y *= -1.0f; // Make up positive
    auto worldPos4 (glm::inverse(m_matrix) * glm::vec4{viewPos, 0, 1});

    return glm::vec2{worldPos4.x, worldPos4.y};
}

void Camera2D::setPos2D (glm::vec2 newPos) {
    m_positionMat = glm::translate(glm::mat4(1.0f), glm::vec3(-newPos, 0.0f));
    m_matrix = m_scaleMat * m_positionMat;
}

std::string_view Camera2D::getName () const noexcept {
    return "Camera2D";
}

void Camera2D::onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) {
    m_scaleMat = glm::scale(m_scaleMat, glm::vec3{m_resolution.x / static_cast<float>(newResolution.x), m_resolution.y / static_cast<float>(newResolution.y), 1.0f});
    m_matrix = m_scaleMat * m_positionMat;
    m_resolution = newResolution;
}

