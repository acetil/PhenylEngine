#pragma once

#include <memory>

#include "graphics/renderer.h"
#include "graphics_properties.h"

namespace phenyl::graphics {
    std::unique_ptr<Renderer> MakeGLRenderer (const GraphicsProperties& properties);
}