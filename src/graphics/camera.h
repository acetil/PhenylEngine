#include "graphics_headers.h"
#include "math_headers.h"
#ifndef CAMERA_H
    #define CAMERA_H
    namespace graphics {
        class Camera {
            private:
            glm::mat4 camMatrix;
            glm::mat4 positionMat;
            glm::mat4 scaleMat;
            public:
            void translate (float x, float y);
            void scale (float scale);
            void setPosition (float x, float y);
            void setScale (float scale);
            glm::mat4 getCamMatrix ();
        };
    }
#endif