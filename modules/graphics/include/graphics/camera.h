#include <string>

#include "graphics_headers.h"
#include "graphics/maths_headers.h"
#ifndef CAMERA_H
    #define CAMERA_H
    namespace graphics {
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
            glm::mat4 getCamMatrix ();
            std::string getUniformName ();
            Camera();
        };
    }
#endif