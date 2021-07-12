#include "graphics/maths_headers.h"

#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H
#ifndef CAMERA_H
namespace graphics{
    class Camera;
}
#endif

namespace game {
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

        void updateCamera (graphics::Camera& camera);
    };
}
#endif
