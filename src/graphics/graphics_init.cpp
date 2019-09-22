#include <stdio.h>
#include <vector>

#include "graphics_headers.h"
#include "graphics.h"
#include "graphics_init.h"
#include "logging/logging.h"
#include "shaders/shaders.h"
#include "textures/image.h"

using namespace graphics;

int graphics::initWindow (GLFWwindow** windowPtr) {
    glewExperimental = true;
    if (!glfwInit()) {
        logging::log(LEVEL_FATAL, "Failed to initialise GLFW!");
        return GRAPHICS_INIT_FAILURE;
    }

    // window hints TODO: update
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, DEFAULT_WINDOW_NAME, NULL, NULL);
    if (window == NULL) {
        logging::log(LEVEL_FATAL, "Failed to open GLFW window! The GPU may not be compatible with OpenGL 3.3!");
        return GRAPHICS_INIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true; // TODO: check if removal affects anything
    if (glewInit() != GLEW_OK) {
        logging::log(LEVEL_FATAL, "Failed to initialise GLEW!");
        return GRAPHICS_INIT_FAILURE;
    }
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    *windowPtr = window;
    logging::log(LEVEL_INFO, "Window initialised successfully!");
    return GRAPHICS_INIT_SUCCESS;
}
std::vector<Image*> getSpriteImages () {
    // temp code
    std::vector<Image*> images;
    images.push_back(new Image("resources/images/test/grass_temp.png", "test1"));
    images.push_back(new Image("resources/images/test/test_texture.png", "test3"));
    images.push_back(new Image("resources/images/test/tier2_ingot-temp.png", "test5"));
    images.push_back(new Image("resources/images/test/temp_ecrys1.png", "test2"));
    return images;
}
int graphics::initGraphics (GLFWwindow* window, Graphics** graphicsPtr) {
    Graphics* graphics = new Graphics(window);
    graphics->setupErrorHandling();
    *graphicsPtr = graphics;
    std::vector<Image*> images = getSpriteImages ();
    graphics->initSpriteAtlas(images);
    for (Image* i : images) {
        delete i;
    }
    logging::log(LEVEL_INFO, "Adding shaders");
    graphics->addShader("default", loadShaderProgram("resources/shaders/vertex.vs", "resources/shaders/fragment.fs", "default"));
    graphics->setCurrentSpriteShader("default");
    //graphics->initBuffer(100);
    return GRAPHICS_INIT_SUCCESS;
}
void graphics::destroyGraphics (Graphics* graphics) {
    delete graphics;
    glfwTerminate();
}

