#include <exception>
#include <utility>

#include "graphics/phenyl_graphics.h"
#include "graphics/opengl/glrenderer.h"

using namespace phenyl::graphics;

std::unique_ptr<Renderer> phenyl::graphics::MakeGLRenderer (const phenyl::graphics::GraphicsProperties& properties) {
    return GLRenderer::Make(properties);
}





