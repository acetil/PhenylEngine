#include <vector>

#include "graphics_headers.h"
#include "graphics_init.h"
#include "logging/logging.h"
#include "shaders/shaders.h"
#include "textures/image.h"
#include "graphics/graphics_handlers.h"
#include "graphics.h"
#include "renderers/glrenderer.h"
#include "font/font_manager.h"

using namespace graphics;
// TODO: update to use exceptions instead of return values
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

    GLFWwindow* window = glfwCreateWindow(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, DEFAULT_WINDOW_NAME, nullptr, nullptr);
    if (window == nullptr) {
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

    glfwSwapInterval(1); // TODO: handle enable/disable vsync

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
    images.push_back(new Image("resources/images/test/stone.png", "test6"));
    images.push_back(new Image("resources/images/test/prismarine_bricks.png", "test7"));
    images.push_back(new Image("resources/images/test/manBlue_gun.png", "test8"));
    images.push_back(new Image("resources/images/test/bullet2.png", "test9"));
    return images;
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
    graphics->addShader("default", loadShaderProgram("resources/shaders/vertex.vs", "resources/shaders/fragment.fs", "default"));
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
    manager.getFace("noto-serif").setFontSize(144);
    manager.getFace("noto-serif").setGlyphs({AsciiGlyphRange});

    return manager;
}

int graphics::initGraphics (GLFWwindow* window, Graphics** graphicsNew) {
    auto renderer = new GLRenderer(window);

    logging::log(LEVEL_INFO, "Adding shaders");
    renderer->addShader("default", loadShaderProgram("resources/shaders/vertex.vs", "resources/shaders/fragment.fs", "default"));
    renderer->addShader("text", loadShaderProgram("resources/shaders/text_vertex.vs", "resources/shaders/text_fragment.fs", "text"));
    renderer->getProgram("text").value()->registerUniform("camera");

    renderer->getProgram("default").value()->registerUniform("camera"); // TODO: update

    auto manager = initFonts();

    auto* graphics = new Graphics(renderer, manager);

    *graphicsNew = graphics;
    logging::log(LEVEL_INFO, "Adding images!");
    std::vector<Image*> images = getSpriteImages(); // TODO: update to match new model system
    std::vector<Model> models;
    models.reserve(images.size());
    for (Image* i : images) {
        models.emplace_back(i->getName(), i);
    }
    graphics->initTextureAtlas("sprite", models);
    images.erase(images.begin(), images.end());


    return GRAPHICS_INIT_SUCCESS;

}

void graphics::destroyGraphics (Graphics* graphics) {
    delete graphics;
    glfwTerminate(); // TODO: move to renderer
}
