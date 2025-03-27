#pragma once

#include <memory>

#include "graphics/backend/renderer.h"
#include "graphics/graphics_properties.h"

namespace phenyl::graphics {
    std::unique_ptr<Renderer> MakeGLRenderer (const GraphicsProperties& properties);
}