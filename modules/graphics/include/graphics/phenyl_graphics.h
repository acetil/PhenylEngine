#pragma once

#include <memory>
#include <string>

#include "ui/ui_manager.h"
#include "graphics_properties.h"

#include "component/component.h"
#include "component/forward.h"
#include "util/optional.h"

namespace phenyl::graphics {
    std::unique_ptr<Renderer> MakeGLRenderer (const GraphicsProperties& properties);
}