#include "graphics/graphics_headers.h"
#ifndef GLCALLBACKS_H
#define GLCALLBACKS_H
namespace graphics {
    void setupGLWindowCallbacks (GLFWwindow* window);
    void removeGLWindowCallbacks (GLFWwindow* window);
}
#endif //GLCALLBACKS_H
