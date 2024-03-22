#include <exception>
#include <utility>

#include "graphics/phenyl_graphics.h"
#include "graphics/graphics.h"
#include "graphics/graphics_init.h"

using namespace phenyl::graphics;

std::unique_ptr<detail::Graphics> phenyl::graphics::MakeGraphics (const phenyl::graphics::GraphicsProperties& properties) {
    GLFWwindow* window = nullptr;
    if (graphics::initWindow(&window, properties) != GRAPHICS_INIT_SUCCESS) {
        PHENYL_ABORT("Window init failure!");
    }

    auto graphics = graphics::initGraphics(window);

    PHENYL_LOGI(detail::GRAPHICS_LOGGER, "Successfully initialised graphics");
    return graphics;
}





