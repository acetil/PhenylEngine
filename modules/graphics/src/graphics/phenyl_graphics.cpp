#include <exception>
#include <utility>

#include "graphics/phenyl_graphics.h"
#include "graphics/graphics.h"
#include "graphics/graphics_init.h"
#include "graphics/glfw/glfw_viewport.h"
#include "graphics/opengl/glrenderer.h"

using namespace phenyl::graphics;

std::unique_ptr<detail::Graphics> phenyl::graphics::MakeGraphics (const phenyl::graphics::GraphicsProperties& properties) {
    auto graphics = std::make_unique<detail::Graphics>(GLRenderer::Make(properties));

    PHENYL_LOGI(detail::GRAPHICS_LOGGER, "Successfully initialised graphics");
    return graphics;
}





