#include <vector>

#include "graphics/graphics_headers.h"
#include "graphics/graphics_init.h"
#include "logging/logging.h"
#include "graphics/textures/image.h"
#include "graphics/graphics.h"
#include "graphics/opengl/glrenderer.h"
#include "graphics/font/font_manager.h"

using namespace graphics;
// TODO: update to use exceptions instead of return values
// TODO: proper error handling
int graphics::initWindow (GLFWwindow** windowPtr) {
    glewExperimental = true;
    if (!glfwInit()) {
        logging::log(LEVEL_FATAL, "Failed to initialise GLFW!");

        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            logging::log(LEVEL_FATAL, "GLFW error code {}: {}", code, glfwError);
        }

        return GRAPHICS_INIT_FAILURE;
    }

    // window hints TODO: update
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, DEFAULT_WINDOW_NAME, nullptr, nullptr);
    if (window == nullptr) {
        logging::log(LEVEL_FATAL, "Failed to open GLFW window! The GPU may not be compatible with OpenGL 3.3!");

        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            logging::log(LEVEL_FATAL, "GLFW error code {}: {}", code, glfwError);
        }

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

    glfwSwapInterval(0); // TODO: handle enable/disable vsync

    *windowPtr = window;
    logging::log(LEVEL_INFO, "Window initialised successfully!");
    return GRAPHICS_INIT_SUCCESS;
}
/*
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
    graphics->addShader("default", loadShaderProgram("resources/shaders/sprite_vertex.vert", "resources/shaders/sprite_fragment.frag", "default"));
    graphics->setCurrentSpriteShader("default");
    //graphics->initBuffer(100);
    return GRAPHICS_INIT_SUCCESS;
}
void graphics::destroyGraphics (Graphics* graphics) {
    delete graphics;
    glfwTerminate();
}*/

FontManager initFonts () {
    FontManager manager;
    manager.addFace("noto-serif", "/usr/share/fonts/noto/NotoSerif-Regular.ttf");
    manager.getFace("noto-serif").setFontSize(72);
    manager.getFace("noto-serif").setGlyphs({AsciiGlyphRange});

    return manager;
}

int graphics::initGraphics (GLFWwindow* window, detail::Graphics::SharedPtr& graphicsNew) {
    auto renderer = std::make_unique<GLRenderer>(window);

    logging::log(LEVEL_INFO, "Adding shaders");
    //renderer->addShader("default", loadShaderProgram("resources/shaders/sprite_vertex.vert", "resources/shaders/sprite_fragment.frag", "default"));
    //renderer->addShader("text", loadShaderProgram("resources/shaders/text_vertex.vert", "resources/shaders/text_fragment.frag", "text"));
    //renderer->getProgram("text").value()->registerUniform("camera");

    //renderer->getProgram("default").value()->registerUniform("camera"); // TODO: update

    //renderer->addShader("default", ShaderBuilder("resources/shaders/sprite_vertex.vert", "resources/shaders/sprite_fragment.frag").addUniform<glm::mat4>("camera"));
    //renderer->addShader("text", ShaderBuilder("resources/shaders/text_vertex.vert", "resources/shaders/text_fragment.frag").addUniform<glm::mat4>("camera"));
    //renderer->addShader("box", ShaderBuilder("resources/shaders/box_vertex.vert", "resources/shaders/box_fragment.frag").addUniform<glm::vec2>("screenSize"));

    auto manager = initFonts();

    auto graphics = std::make_shared<detail::Graphics>(std::move(renderer), manager);

    graphicsNew = graphics;


    return GRAPHICS_INIT_SUCCESS;

}

void graphics::destroyGraphics (const detail::Graphics::SharedPtr& graphics) {
    graphics->deleteWindowCallbacks();
    //delete graphics;
    //glfwTerminate(); // TODO: move to renderer
}
