#include "graphics/detail/loggers.h"

using namespace phenyl;

Logger graphics::detail::GRAPHICS_LOGGER{"GRAPHICS"};
Logger graphics::detail::SHADER_LOGGER{"SHADER", GRAPHICS_LOGGER};