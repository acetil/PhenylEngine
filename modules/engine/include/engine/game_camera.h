#pragma once

#include "graphics/maths_headers.h"

#ifndef CAMERA_H
namespace phenyl::graphics {
    class Camera;
}
#endif

namespace phenyl::game {
    class GameCamera {
    private:
        glm::vec2 currentPos{};
        glm::vec2 currentScale{1.0f, 1.0f};
        glm::vec2 targetPos{};
        glm::vec2 targetScale{1.0f, 1.0f};
    public:
        void setPos (glm::vec2 pos);
        void translate (glm::vec2 vec);
        void setScale (float scaleX, float scaleY);
        void scaleBy (float scaleX, float scaleY);

        glm::vec2 getWorldPos (glm::vec2 screenPos); // Asumes screenPos is -1 to 1

        void updateCamera (graphics::Camera& camera);
    };
}
