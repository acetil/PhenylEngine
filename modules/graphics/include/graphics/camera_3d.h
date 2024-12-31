#pragma once

#include "viewport.h"
#include "core/iresource.h"
#include "core/maths/3d/transform.h"

namespace phenyl::graphics {
    class Camera3D : public core::IResource, public IViewportUpdateHandler {
    private:
        glm::vec2 resolution;
    public:
        float nearClippingPlane = 0.1f;
        float farClippingPlane = 100.0f;
        core::Transform3D transform{};
        float fov = 60.0f;

        Camera3D (glm::vec2 resolution);

        void lookAt (glm::vec3 target, glm::vec3 upAxis = core::Quaternion::UpVector) noexcept;

        glm::mat4 viewMatrix () const noexcept;
        glm::mat4 perspectiveMatrix () const noexcept;
        glm::mat4 cameraMatrix () const noexcept;

        void onViewportResize (glm::ivec2 oldResolution, glm::ivec2 newResolution) override;
        std::string_view getName() const noexcept override;
    };
}
