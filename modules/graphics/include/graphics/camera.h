#pragma once

#include <string>

#include "graphics_headers.h"
#include "graphics/maths_headers.h"

namespace phenyl::graphics {
    class Camera {
    private:
        glm::mat4 camMatrix{};
        glm::mat4 positionMat{};
        glm::mat4 scaleMat{};
    public:
        [[maybe_unused]] void translate (float x, float y);

        [[maybe_unused]] void scale (float scale);

        [[maybe_unused]] void setPosition (float x, float y);

        [[maybe_unused]] void setScale (float scale);
        [[nodiscard]] glm::mat4 getCamMatrix () const ;
        static std::string getUniformName () ;
        Camera();
    };
}
