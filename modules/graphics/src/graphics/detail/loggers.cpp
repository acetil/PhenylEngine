#include "graphics/detail/loggers.h"

using namespace phenyl;

Logger graphics::detail::GRAPHICS_LOGGER{"GRAPHICS", PHENYL_LOGGER};
Logger graphics::detail::SHADER_LOGGER{"SHADER", GRAPHICS_LOGGER};
Logger graphics::detail::RENDERER_LOGGER{"RENDERER", GRAPHICS_LOGGER};
