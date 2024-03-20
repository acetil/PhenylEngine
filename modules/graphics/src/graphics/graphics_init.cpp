#include <vector>

#include "graphics/graphics_headers.h"
#include "graphics/graphics_init.h"
#include "logging/logging.h"
#include "graphics/textures/image.h"
#include "graphics/graphics.h"
#include "graphics/opengl/glrenderer.h"
#include "graphics/font/font_manager.h"

using namespace phenyl::graphics;
// TODO: update to use exceptions instead of return values
// TODO: proper error handling

static phenyl::Logger LOGGER{"GRAPHICS_INIT", detail::GRAPHICS_LOGGER};

int phenyl::graphics::initWindow (GLFWwindow** windowPtr, const GraphicsProperties& properties) {
    glewExperimental = true;
    if (!glfwInit()) {
        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            PHENYL_LOGE(LOGGER, "glfwInit() error code {}: {}", code, glfwError);
        }

        PHENYL_ABORT("Failed to initialise GLFW!");

        return GRAPHICS_INIT_FAILURE;
    }

    // window hints TODO: update
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(properties.getWindowWidth(), properties.getWindowHeight(), properties.getWindowTitle().c_str(), nullptr, nullptr);
    if (window == nullptr) {
        const char* glfwError;
        int code;
        if ((code = glfwGetError(&glfwError))) {
            PHENYL_LOGE(LOGGER, "glfwCreatWindow() error code {}: {}", code, glfwError);
        }

        PHENYL_ABORT("Failed to open GLFW window! The GPU may not be compatible with OpenGL 3.3!");

        return GRAPHICS_INIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true; // TODO: check if removal affects anything
    if (glewInit() != GLEW_OK) {
        PHENYL_ABORT("Failed to initialise GLEW!");
        return GRAPHICS_INIT_FAILURE;
    }

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(properties.getVsync() ? 1 : 0); // TODO: handle enable/disable vsync

    *windowPtr = window;
    PHENYL_LOGD(LOGGER, "Window initialised successfully!");
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

std::unique_ptr<detail::Graphics> phenyl::graphics::initGraphics (GLFWwindow* window) {
    auto renderer = std::make_unique<GLRenderer>(window);
    //renderer->addShader("default", loadShaderProgram("resources/shaders/sprite_vertex.vert", "resources/shaders/sprite_fragment.frag", "default"));
    //renderer->addShader("text", loadShaderProgram("resources/shaders/text_vertex.vert", "resources/shaders/text_fragment.frag", "text"));
    //renderer->getProgram("text").value()->registerUniform("camera");

    //renderer->getProgram("default").value()->registerUniform("camera"); // TODO: update

    //renderer->addShader("default", ShaderBuilder("resources/shaders/sprite_vertex.vert", "resources/shaders/sprite_fragment.frag").addUniform<glm::mat4>("camera"));
    //renderer->addShader("text", ShaderBuilder("resources/shaders/text_vertex.vert", "resources/shaders/text_fragment.frag").addUniform<glm::mat4>("camera"));
    //renderer->addShader("box", ShaderBuilder("resources/shaders/box_vertex.vert", "resources/shaders/box_fragment.frag").addUniform<glm::vec2>("screenSize"));

    return std::make_unique<detail::Graphics>(std::move(renderer), initFonts());

}