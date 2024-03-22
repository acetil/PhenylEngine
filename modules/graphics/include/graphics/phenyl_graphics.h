#pragma once

#include <memory>
#include <string>

#include "renderlayer/graphics_layer.h"
#include "ui/ui_manager.h"
#include "graphics_properties.h"

#include "component/component.h"
#include "component/forward.h"
#include "util/optional.h"

namespace phenyl::graphics {
    namespace detail {
        class Graphics;
    }

    class Renderer;

    std::unique_ptr<detail::Graphics> MakeGraphics (const GraphicsProperties& properties);
}